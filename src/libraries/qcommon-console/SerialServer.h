#ifndef SERIALSERVER
#define SERIALSERVER

// Qt
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QSharedPointer>
#include <QSerialPortInfo>

// QCommon
#include "PreferencesBase.h"

// Serial
#include "SerialPort.h"

typedef QList<QSerialPortInfo> SerialPorts;
typedef SerialPorts::iterator SerialPortIter;

class SerialServer;
class SerialServerInit;

typedef QSharedPointer<SerialServer> SPSerialServer;

class SerialServer :
	public QObject
{
	Q_OBJECT

	friend class SerialServerInit;
public:
	~SerialServer();

	static SPSerialServer GetSerialServer(void);

	static QString GetVersion()
	{
		return "1.0.0.1";
	}

	int ComPortCount(void);

	SerialPorts GetSerialPorts(void);
	
	SerialPort* GetSerialPort(const QString& systemPath, PreferencesBase* preferences = nullptr);
	SerialPort* GetSerialPort(const QSerialPortInfo& serialPortInfo, PreferencesBase* preferences = nullptr);

protected:
	SerialServer();


	QMap<QString, QString>			_assignedPorts;
	QMap<QString, QSerialPortInfo>	_openPorts;

	static SPSerialServer			_serialServer;

protected slots:
	void on_PortOpen(const QSerialPortInfo& serialPortInfo);
	void on_PortClose(const QSerialPortInfo& serialPortInfo);
};

#endif // SERIALSERVER

