#pragma once

#include <QObject>

class MessageClass : public QObject
{
    Q_OBJECT
private:
    QString senderName;
    QString content;
    QString timestamp;
    bool isOwnMessage;
public:
    MessageClass(QString senderName, QString content, QString timestamp, bool isOwnMessage):
        senderName(senderName), content(content), timestamp(timestamp), isOwnMessage(isOwnMessage){}
    QString Get_senderName(){return senderName;}
    QString Get_content(){return content;}
    QString Get_timestamp(){return timestamp;}
    bool Get_isOwnMessage(){return isOwnMessage;}

    void Set_senderName(const QString& _senderName);
    void Set_content(const QString& _content);
    void Set_timestamp(const QString& _timestamp);
    void Set_isOwnMessage(const bool& _isOwnMessage);

};