// Confidential and Proprietary – Qualcomm Technologies, Inc.

// NO PUBLIC DISCLOSURE PERMITTED:  Please report postings of this software on public servers or websites
// to: DocCtrlAgent@qualcomm.com.

// RESTRICTED USE AND DISCLOSURE:
// This software contains confidential and proprietary information and is not to be used, copied, reproduced, modified
// or distributed, in whole or in part, nor its contents revealed in any manner, without the express written permission
// of Qualcomm Technologies, Inc.

// Qualcomm is a trademark of QUALCOMM Incorporated, registered in the United States and other countries. All
// QUALCOMM Incorporated trademarks are used with permission.

// This software may be subject to U.S. and international export, re-export, or transfer laws.  Diversion contrary to U.S.
// and international law is strictly prohibited.

// Qualcomm Technologies, Inc.
// 5775 Morehouse Drive
// San Diego, CA 92121 U.S.A.
// Copyright © 2013-2016 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson msimpson@qti.qualcomm.com
*/

#include "LibUsbEngine.h"
#include "LibUsbInitializer.h"

static quint32 kLinuxUsbEngineSymbol(0); // Prevents clang compiler from warning

void LinuxLinkerNoSymbolRemedy()
{
   kLinuxUsbEngineSymbol = -1;
}

#ifdef Q_OS_LINUX

#include "LibUsbEngine.h"

//Qt
#include <QDir>
#include <QSet>
#include <QString>

// QtUsb
#include "LibUsbInitializer.h"
#include "UsbDeviceItem.h"

const QString kUsbSystemPath("/sys/bus/usb/devices/");

typedef QList<quint8> PortList;

void SplitBusFromPorts
(
const QString& path,
quint8& bus,
PortList& ports
)
{
   ports.clear();

   QStringList splitStr = path.split("-", QString::SkipEmptyParts);

   if (splitStr.count() >= 2)
   {
      bus = (quint8)splitStr.at(0).toInt();

      QStringList portsStr = splitStr.at(1).split(".", QString::SkipEmptyParts);

      QStringList::iterator portStr = portsStr.begin();
      while (portStr != portsStr.end())
      {
         ports.push_back((quint8)(*portStr).toInt());
         portStr++;
      }
   }
}

LinuxUsbEngine::LinuxUsbEngine()
{
   LibUsbInitialize();

   connect(&m_timer, SIGNAL(timeout()), this, SLOT(onTimerTimeout()));

   m_timer.start(500);
}

LinuxUsbEngine::~LinuxUsbEngine()
{
}

void LinuxUsbEngine::onTimerTimeout()
{
   ProcessUsbList();
}

void LinuxUsbEngine::ProcessUsbList()
{
   m_buildDevices.clear();

   QStringList subDirs = QDir(kUsbSystemPath).entryList(QStringList(), QDir::AllDirs | QDir::NoDotAndDotDot, QDir::Name);

   QStringList::iterator subDir = subDirs.begin();
   while (subDir != subDirs.end())
   {
      if ((*subDir).startsWith("usb"))
         AddBus(*subDir);

      subDir++;
   }

   subDir = subDirs.begin();
   while (subDir != subDirs.end())
   {
      if (subDir->contains("usb") == false)
      {
         QString path = *subDir;

         if (path.contains(":") == false)
         {
            ProcessDevice(path);
         }
      }

      subDir++;
   }

   MergeUsbList();
}

void LinuxUsbEngine::MergeUsbList()
{
   QStringList buildKeys;
   QStringList existingKeys;
   QSet<QString> removedDevices;
   QSet<QString> addedDevices;

   {	// Scoping bracket, DO NOT REMOVE
      QMutexLocker lock(&m_listMutex);
      existingKeys = m_devices.keys();
      buildKeys = m_buildDevices.keys();

      // Subtract the current set from the old set to find items that no longer exist
      removedDevices = QSet<QString>::fromList(existingKeys).subtract(QSet<QString>::fromList(buildKeys));

      // Subtract the new set from the old set to find items the old set doesn't know about
      addedDevices = QSet<QString>::fromList(buildKeys).subtract(QSet<QString>::fromList(existingKeys));

      if (removedDevices.isEmpty() == true && addedDevices.isEmpty() == true)
          return; // nothing to do


      QList<QString>::iterator iter = m_quietDevices.begin();
      while (iter != m_quietDevices.end())
      {
          if (m_devices.contains(*iter))
          {
              addedDevices.remove(*iter);
              m_buildDevices.remove(*iter);
              m_devices.remove(*iter);

              iter = m_quietDevices.erase(iter);
          }
          else
              iter++;
      }

      m_devices = m_buildDevices;
   }

   QSet<QString>::iterator devicePath;

   devicePath = removedDevices.begin();
   while (devicePath != removedDevices.end())
   {
      emit DeviceRemoved(*devicePath);
      devicePath++;
   }

   devicePath = addedDevices.begin();
   while (devicePath != addedDevices.end())
   {
      emit DeviceAdded(*devicePath);
      devicePath++;
   }
}

void LinuxUsbEngine::AddBus
(
const QString& path
)
{
   QString busNum = QString(path).remove("usb");
   quint32 busIndex = busNum.toUInt();

   QString busFolder = QDir::cleanPath(kUsbSystemPath + QDir::separator() + path);
   UsbDeviceItem bus = UsbDeviceItem(new _UsbDeviceItem);

   bus->m_busID = busIndex;
   bus->m_deviceType = ePciDevice;
   bus->m_fsPath = QString::number(busIndex);

   QStringList deviceFiles = QDir(busFolder).entryList(QStringList(), QDir::Files | QDir::NoDotAndDotDot, QDir::Name);

   QStringList::iterator deviceFileIter = deviceFiles.begin();
   while (deviceFileIter != deviceFiles.end())
   {
      QString deviceFileName(*deviceFileIter);
      QString deviceFilePath(busFolder + QDir::separator() + deviceFileName);
      QFile deviceFile(deviceFilePath);

      if (deviceFile.open(QIODevice::ReadOnly | QIODevice::Text) == true)
      {
         QByteArray fileContents = deviceFile.readAll().trimmed();

         if (fileContents.isEmpty() == false)
            SetVariable(bus, deviceFileName.toLower(), fileContents);

         deviceFile.close();
      }

      deviceFileIter++;
   }

   m_buildDevices[bus->m_fsPath] = bus;
}

void LinuxUsbEngine::SetVariable
(
UsbDeviceItem& usbDevice,
const QString& variableName,
const QByteArray& value
)
{
   bool okay;

   if (variableName == "bcddevice")
      usbDevice->m_bcdDeviceRev = value.toInt();
   else if (variableName == "serial")
      usbDevice->m_serial = QString(value);
   else if (variableName == "idvendor")
      usbDevice->m_vendorID = value.toInt(&okay, 16);
   else if (variableName == "idproduct")
      usbDevice->m_productID = value.toInt(&okay, 16);
   else if (variableName == "bdeviceclass")
      usbDevice->m_class = value.toInt();
   else if (variableName == "bdevicesubclass")
      usbDevice->m_subClass = value.toInt();
   else if (variableName == "manufacturer")
      usbDevice->m_manufacturer = QString(value);
   else if (variableName == "product")
      usbDevice->m_product = QString(value);
   else if (variableName == "bmaxpacketsize0")
      usbDevice->m_ep0maxPacketSize = value.toInt();
   else if (variableName == "speed")
   {
      switch (value.toInt())
      {
      case 1: usbDevice->m_speed = _UsbDeviceItem::eLow; break;
      case 12: usbDevice->m_speed = _UsbDeviceItem::eFull; break;
      case 480: usbDevice->m_speed = _UsbDeviceItem::eHigh; break;
      case 3: usbDevice->m_speed = _UsbDeviceItem::eSuper; break;
      default: break;
      }
   }
   else if (variableName == "authorized")
   {
      usbDevice->m_authorized = value.toInt() == 0 ? false : true;
   }
   else if (variableName == "authorized_default")
   {
      usbDevice->m_authorized_default = value.toInt() == 0 ? false : true;
   }
   else if (variableName == "bdeviceprotocol")
   {
      usbDevice->m_protocol = value.toInt();
   }
   else
   {
      QString foo;

      foo = "unhandled";
   }

   //	m_deviceID = copyMe.m_deviceID;
   //	m_bcdUSB = copyMe.m_bcdUSB;
   //	m_protocol = copyMe.m_protocol;
}

void LinuxUsbEngine::ProcessDevice
(
const QString& path
)
{
   UsbDeviceItem usbDevice = UsbDeviceItem(new _UsbDeviceItem);

   quint8 bus;
   QList<quint8> ports;

   SplitBusFromPorts(path, bus, ports);

   QString fspath(QString::number(bus));

   QList<quint8>::iterator portIter = ports.begin();
   while (portIter != ports.end())
   {
      fspath += "." + QString::number(*portIter);
      portIter++;
   }

   usbDevice->m_fsPath = fspath;
   usbDevice->m_deviceKey = fspath;

   QString devicePath(QDir::cleanPath(kUsbSystemPath + QDir::separator() + path));
   QStringList deviceFiles = QDir(devicePath).entryList(QStringList(), QDir::Files | QDir::NoDotAndDotDot, QDir::Name);

   QStringList::iterator deviceFileIter = deviceFiles.begin();
   while (deviceFileIter != deviceFiles.end())
   {
      QString deviceFileName(*deviceFileIter);
      QString deviceFilePath(QDir::cleanPath(devicePath + QDir::separator() + deviceFileName));
      QFile deviceFile(deviceFilePath);

      if (deviceFile.open(QIODevice::ReadOnly | QIODevice::Text) == true)
      {
         QByteArray fileContents = deviceFile.readAll().trimmed();

         if (fileContents.isEmpty() == false)
            SetVariable(usbDevice, deviceFileName.toLower(), fileContents);

         deviceFile.close();
      }

      deviceFileIter++;
   }

   usbDevice->m_busID = bus;

   if (usbDevice->m_class == 0x09)
      usbDevice->m_deviceType = eHub;
   else
      usbDevice->m_deviceType = eDevice;

   m_buildDevices[usbDevice->m_deviceKey] = usbDevice;
}

#endif // Q_OS_LINUX
