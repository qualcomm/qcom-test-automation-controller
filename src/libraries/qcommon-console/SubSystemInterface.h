#ifndef SUBSYSTEMINTERFACE_H
#define SUBSYSTEMINTERFACE_H

// QCommon
#include "DiagMessages/DiagMessage.h"
#include "ReceiveInterface.h"
#include "SendInterface.h"

// Qt
#include <QList>

class SubSystemInterface :
	public ReceiveInterface
{
public:
	SubSystemInterface() = default;
	~SubSystemInterface() = default;

	virtual bool Initialize(SendInterface* sendInterface) = 0;
	virtual void Shutdown() = 0;
	virtual void GetInitialRequests(DiagMessages& diagMessages) = 0;
	virtual bool HandledMessage(const DiagMessage& dataPacket, bool& shouldRecord) = 0;

protected:
	SendInterface*				_sendInterface{Q_NULLPTR};
};

typedef QList<SubSystemInterface*> SubSystems;
typedef SubSystems::iterator SybSystermIter;

#endif // SUBSYSTEMINTERFACE_H
