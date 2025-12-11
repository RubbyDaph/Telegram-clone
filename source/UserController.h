#pragma once

#include <QObject>
#include <QString>
#include "NetworkManager.h"
#include "ChatList.h"
#include "DialogClass.h"

class UserController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(ChatList* chatList READ GetChatList CONSTANT)
    Q_PROPERTY(DialogModel* dialogModel READ GetDialogModel CONSTANT)
public:
    explicit UserController(QObject *parent = nullptr);

    Q_INVOKABLE ChatList* GetChatList() const;
    Q_INVOKABLE DialogModel* GetDialogModel() const;

    Q_INVOKABLE void SetCurrentChat(const int& chatID);
    Q_INVOKABLE void StartMessaging();
    Q_INVOKABLE void Logout(const QString& userAddress);
    Q_INVOKABLE void SendMessage(const QString& message);
    Q_INVOKABLE void CreateChat(const QString& contactName);
private slots:
    void OnMessageReceived(const QString& receivedMessage, const QString& sender);
    void OnMessageSent(const QString& sentMessage, const QString& messageID, bool success);
    void OnConnectionStatusChanged(bool isOnline);
    void OnTypingStatusChanged(bool isTyping, const QString& userAddress);
    void OnConnectionFail(const QString& reason);
signals:
    void MessageReceived(const QString& receivedMessage, const QString& sender);
    void MessageSent(const QString& sentMessage, const QString& messageID, bool success);
    void ConnectionStatusChange(bool isOnline);
    void TypingStatusChange(bool isTyping, const QString& userAddress);
    void ConnectionFail(const QString& reason);
    void DialogModelChanged(DialogModel* dialogModel);
private:
    NetworkManager* networkManager;
    ChatList* chatList;
    DialogModel* dialogModel;
    MessageClass* messageClass;
};