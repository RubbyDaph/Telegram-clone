#pragma once

#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>
#include <QJsonDocument>

class NetworkManager : public QObject
{
    Q_OBJECT
public:
    explicit NetworkManager(QObject* parent = nullptr) : QObject(parent) {}
public slots:
    void ConnectToHost(const QString& userAddress, quint16 port);
    void DisconnectUser(const QString& userAddress, quint16 port);

    void StartServer(quint16 port);

    void SendMessage(const QString& message, const QString& userAddress);

signals:
    void ConnectedToHost();
    void ConnectionFail();
    void Disconnected();

    void TypingStatusChanged(const QString& userAddress, bool isTyping);

    void UserStatusChanged(const QString& userAddress, bool isOnline);

    void UserConnected(const QString& userAddress);
    void UserDisconnected(const QString& userAddress, const QString& reason);

    void MessageReceived(const QString& userAddress, const QString& message, const QDateTime& timestamp);
    void MessageSent(const QString& userAddress, const QString& message, const QDateTime& timestamp, bool success);

};