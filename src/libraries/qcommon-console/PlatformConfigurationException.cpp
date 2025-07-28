#include "PlatformConfigurationException.h"

PlatformConfigurationException::PlatformConfigurationException(QString const& message) :
    _message(message)
{

}

PlatformConfigurationException::~PlatformConfigurationException()
{

}

QString PlatformConfigurationException::getMessage() const
{
    return _message;
}
