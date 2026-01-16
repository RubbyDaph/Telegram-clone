#pragma once

#include <QObject>
#include <QString>
#include "NetworkManager.h"
#include "ChatList.h"
#include "DialogClass.h"
#include <QSettings>

class UserController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(ChatList* chatList READ GetChatList CONSTANT)
    Q_PROPERTY(DialogModel* dialogModel READ GetDialogModel CONSTANT)
    Q_PROPERTY(QString username READ GetUsername WRITE SetUsername NOTIFY UsernameChanged)
public:
    explicit UserController(QObject *parent = nullptr);

    Q_INVOKABLE ChatList* GetChatList() const;
    Q_INVOKABLE DialogModel* GetDialogModel() const;

    Q_INVOKABLE void SetCurrentChat(const QString& chatID);
    Q_INVOKABLE void StartMessaging();
    Q_INVOKABLE void Logout(const QString& userAddress);
    Q_INVOKABLE void SendMessage(const QString& message);
    Q_INVOKABLE void CreateChat(const QString& contactName);
    void SetUsername(const QString& name);
    QString GetUsername() const {return username;}
private slots:
    void OnMessageReceived(const QString& peerAddress, const QString& message, const QDateTime& timestamp);
    void OnMessageSent(const QString& sentMessage, const QString& messageID, bool isOwnMessage);
    void onPeerTypingStatusChanged(const QString& peerId, bool isTyping);
    void onPeerPresenceChanged(const QString& peerId, bool isOnline);
    void OnConnectionFail(const QString& reason);
    void OnPeerConnected(const QString& peerID, const QString& peerAddress, const QString& peerName);
    void OnPeerDisconnected(const QString& peerID, const QString& peerAddress);
signals:
    void MessageReceived(const QString& message, const QString senderName);
    void MessageSent(const QString& sentMessage, const QString& messageID, bool isOwnMessage);
    void ConnectionStatusChange(bool isOnline);
    void TypingStatusChange(bool isTyping, const QString& userAddress);
    void ConnectionFail(const QString& reason);
    void DialogModelChanged(DialogModel* dialogModel);
    void UsernameChanged();
    void UsernameChangedForNetwork(const QString& newName);
private:
    NetworkManager* networkManager;
    ChatList* chatList;
    DialogModel* dialogModel;
    MessageClass* messageClass;
    QString username;
    QString myUuid;
};