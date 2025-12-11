#include "MessageClass.h"

void MessageClass::Set_senderName(const QString& _senderName)
{
    senderName = _senderName;
}
void MessageClass::Set_content(const QString &_content)
{
    content = _content;
}
void MessageClass::Set_timestamp(const QString &_timestamp)
{
    timestamp = _timestamp;
}
void MessageClass::Set_isOwnMessage(const bool &_isOwnMessage)
{
    isOwnMessage = _isOwnMessage;
}



