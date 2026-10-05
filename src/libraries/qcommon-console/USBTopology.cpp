// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "USBTopology.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>

#ifdef Q_OS_WIN

#include <windows.h>
#include <setupapi.h>
#include <cfgmgr32.h>

// {4D36E978-E325-11CE-BFC1-08002BE10318} - GUID_DEVCLASS_PORTS
static const GUID kGuidDevClassPorts =
{
	0x4d36e978, 0xe325, 0x11ce, { 0xbf, 0xc1, 0x08, 0x00, 0x2b, 0xe1, 0x03, 0x18 }
};

static bool isUsbHubNode(DEVINST devInst)
{
	char service[64]{};
	ULONG length = sizeof(service);

	if (CM_Get_DevNode_Registry_PropertyA(devInst, CM_DRP_SERVICE, Q_NULLPTR, service, &length, 0) != CR_SUCCESS)
		return false;

	QByteArray serviceName(service);
	return serviceName.startsWith("usbhub", Qt::CaseInsensitive);
}

static QByteArray walkUpToParentHub(DEVINST devInst)
{
	DEVINST current = devInst;

	for (int tier{0}; tier < 10; tier++)
	{
		DEVINST parent;
		if (CM_Get_Parent(&parent, current, 0) != CR_SUCCESS)
			return QByteArray();

		if (isUsbHubNode(parent))
		{
			char deviceId[MAX_DEVICE_ID_LEN]{};
			if (CM_Get_Device_IDA(parent, deviceId, MAX_DEVICE_ID_LEN, 0) == CR_SUCCESS)
				return QByteArray(deviceId);

			return QByteArray();
		}

		current = parent;
	}

	return QByteArray();
}

QByteArray _USBTopology::parentHubIdForFtdiDevice(const QByteArray& ftdiSerialNumber)
{
	if (ftdiSerialNumber.isEmpty())
		return QByteArray();

	HDEVINFO devInfoSet = SetupDiGetClassDevsA(Q_NULLPTR, "USB", Q_NULLPTR, DIGCF_PRESENT | DIGCF_ALLCLASSES);
	if (devInfoSet == INVALID_HANDLE_VALUE)
		return QByteArray();

	QByteArray result;
	SP_DEVINFO_DATA devInfoData{};
	devInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

	for (DWORD index{0}; SetupDiEnumDeviceInfo(devInfoSet, index, &devInfoData); index++)
	{
		char instanceId[MAX_DEVICE_ID_LEN]{};
		if (SetupDiGetDeviceInstanceIdA(devInfoSet, &devInfoData, instanceId, MAX_DEVICE_ID_LEN, Q_NULLPTR) == FALSE)
			continue;

		if (QByteArray(instanceId).contains(ftdiSerialNumber))
		{
			result = walkUpToParentHub(devInfoData.DevInst);
			break;
		}
	}

	SetupDiDestroyDeviceInfoList(devInfoSet);

	return result;
}

QByteArray _USBTopology::parentHubIdForSerialPort(const QByteArray& portName)
{
	if (portName.isEmpty())
		return QByteArray();

	HDEVINFO devInfoSet = SetupDiGetClassDevsA(&kGuidDevClassPorts, Q_NULLPTR, Q_NULLPTR, DIGCF_PRESENT);
	if (devInfoSet == INVALID_HANDLE_VALUE)
		return QByteArray();

	QByteArray result;
	SP_DEVINFO_DATA devInfoData{};
	devInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

	for (DWORD index{0}; SetupDiEnumDeviceInfo(devInfoSet, index, &devInfoData); index++)
	{
		HKEY deviceKey = SetupDiOpenDevRegKey(devInfoSet, &devInfoData, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_READ);
		if (deviceKey == INVALID_HANDLE_VALUE)
			continue;

		char portNameValue[64]{};
		DWORD portNameSize = sizeof(portNameValue);
		LONG queryResult = RegQueryValueExA(deviceKey, "PortName", Q_NULLPTR, Q_NULLPTR, reinterpret_cast<LPBYTE>(portNameValue), &portNameSize);
		RegCloseKey(deviceKey);

		if (queryResult == ERROR_SUCCESS && QByteArray(portNameValue) == portName)
		{
			result = walkUpToParentHub(devInfoData.DevInst);
			break;
		}
	}

	SetupDiDestroyDeviceInfoList(devInfoSet);

	return result;
}

#endif // Q_OS_WIN

#ifdef Q_OS_LINUX

static const QByteArray kSysfsUsbDevicesPath{"/sys/bus/usb/devices"};

static QByteArray readSysfsAttribute(const QString& deviceDirPath, const QString& attributeName)
{
	QFile attributeFile(deviceDirPath + "/" + attributeName);
	if (attributeFile.open(QIODevice::ReadOnly) == false)
		return QByteArray();

	return attributeFile.readAll().trimmed();
}

// Given a USB device's own sysfs busid (e.g. "3-1.4" or root "usb3"), derive
// the busid of its parent hub (e.g. "3-1", or the host controller's root hub).
static QByteArray deriveParentHubId(const QString& busId)
{
	int lastDot = busId.lastIndexOf('.');
	if (lastDot > 0)
		return busId.left(lastDot).toLatin1();

	int dash = busId.indexOf('-');
	if (dash > 0)
		return ("usb" + busId.left(dash)).toLatin1();

	return QByteArray();
}

QByteArray _USBTopology::parentHubIdForFtdiDevice(const QByteArray& ftdiSerialNumber)
{
	if (ftdiSerialNumber.isEmpty())
		return QByteArray();

	QDir usbDevicesDir(kSysfsUsbDevicesPath);
	const auto entries = usbDevicesDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

	for (const auto& entry: entries)
	{
		if (entry.contains(':')) // interface node, not a device node
			continue;

		QString devicePath = usbDevicesDir.absoluteFilePath(entry);
		QByteArray serial = readSysfsAttribute(devicePath, "serial");
		if (serial.isEmpty())
			continue;

		if (serial.contains(ftdiSerialNumber) || ftdiSerialNumber.contains(serial))
			return deriveParentHubId(entry);
	}

	return QByteArray();
}

QByteArray _USBTopology::parentHubIdForSerialPort(const QByteArray& portName)
{
	if (portName.isEmpty())
		return QByteArray();

	QString ttyName = QFileInfo(QString::fromLatin1(portName)).fileName();
	QString deviceLink = "/sys/class/tty/" + ttyName + "/device";

	QFileInfo deviceLinkInfo(deviceLink);
	if (deviceLinkInfo.exists() == false)
		return QByteArray();

	// The tty's "device" symlink points at the USB interface node, which is
	// nested directly under the USB device node in sysfs.
	QString interfacePath = deviceLinkInfo.canonicalFilePath();
	QString devicePath = QFileInfo(interfacePath).dir().absolutePath();
	QString deviceBusId = QFileInfo(devicePath).fileName();

	return deriveParentHubId(deviceBusId);
}

#endif // Q_OS_LINUX
