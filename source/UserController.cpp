#include "UserController.h"
#include<QDebug>

UserController::UserController(QObject *parent)
    : QObject(parent)
    , networkManager(nullptr)
    , chatList(nullptr)
    , dialogModel(nullptr)
    , messageClass(nullptr)
{
    networkManager = new NetworkManager(this);
    chatList = new ChatList(this);
    dialogModel = new DialogModel(this);
    messageClass = new MessageClass("", "", "", false);

    connect(networkManager, &NetworkManager::peerTypingStatusChanged, this, &UserController::onPeerTypingStatusChanged);
    connect(this, &UserController::DialogModelChanged, dialogModel, &DialogModel::OnDialogModelChanged);
    connect(networkManager, &NetworkManager::MessageReceived, this, &UserController::OnMessageReceived);
    connect(networkManager, &NetworkManager::peerTypingStatusChanged, this, &UserController::onPeerTypingStatusChanged);
    connect(networkManager, &NetworkManager::peerPresenceChanged, this, &UserController::onPeerPresenceChanged);
    connect(networkManager, &NetworkManager::PeerConnected, this, &UserController::OnPeerConnected);
    connect(networkManager, &NetworkManager::PeerDisconnected, this, &UserController::OnPeerDisconnected);
    connect(this, &UserController::UsernameChangedForNetwork, networkManager, &NetworkManager::onUsernameChanged);
    if (!username.isEmpty())
    {
        emit UsernameChangedForNetwork(username);
    }
    myUuid = networkManager->GetMyUuid();

    QSettings settings;
    username = settings.value("user/name", "User").toString();
}


void UserController::SetCurrentChat(const QString &chatID)
{
    dialogModel = chatList->getDialogModel(chatID);

    emit DialogModelChanged(dialogModel);
}

void UserController::StartMessaging()
{
    qDebug() << "StartMessaging called";

    bool started = networkManager->StartP2PNode(0);

    if (!started) {
        emit ConnectionFail("Failed to start P2P node - no available ports");
    } else {
        qDebug() << "P2P node started successfully";
    }
}

void UserController::Logout(const QString &userAddress)
{
    qDebug() << "Logout called for address:" << userAddress;

    networkManager->StopP2PNode();

    emit ConnectionStatusChange(false);
}

void UserController::SendMessage(const QString &message)
{
    if (!dialogModel || message.trimmed().isEmpty()) return;

    MessageClass* newMessage = new MessageClass(
        "You",  // senderName
        message, // content
        QDateTime::currentDateTime().toString("hh:mm"), // timestamp
        true     // isOwnMessage
    );

    dialogModel->AddMessage(newMessage);
    QString messageId = QUuid::createUuid().toString();
    emit MessageSent(message, messageId, true);
}


void UserController::OnMessageReceived(const QString& peerAddress, const QString& message, const QDateTime& timestamp)
{
    qDebug() << "Message received from" << peerAddress << ":" << message;

    // Определяем имя отправителя на основе peerAddress
    QString senderName;

    if (peerAddress.startsWith("temp_")) {
        // Временный ID (до hello-обмена) - показываем адрес
        senderName = "User (" + peerAddress.mid(5) + ")";
    } else if (peerAddress.startsWith("{")) {
        // UUID в формате {aaaaaaaa-bbbb-...} - сокращаем для отображения
        senderName = "User_" + peerAddress.mid(1, 8);
    } else {
        // Любой другой формат - используем как есть
        senderName = peerAddress;
    }

    // Создаем объект сообщения (НЕ наше)
    MessageClass* receivedMessage = new MessageClass(
        senderName,
        message,
        timestamp.toString("hh:mm"),
        false // isOwnMessage = false (не наше сообщение)
    );

    // TODO: Здесь нужно найти правильный DialogModel для этого отправителя
    // и добавить сообщение туда

    // Если dialogModel существует, добавляем сообщение
    if (dialogModel) {
        dialogModel->AddMessage(receivedMessage);
    }

    emit MessageReceived(message, senderName);
}

void UserController::OnMessageSent(const QString &sentMessage, const QString &messageID, bool isOwnMessage)
{

}

void UserController::onPeerPresenceChanged(const QString& peerId, bool isOnline)
{
    qDebug() << "OnConnectionStatusChanged called: " << isOnline;
}

void UserController::onPeerTypingStatusChanged(const QString& peerId, bool isTyping)
{
    qDebug() << "OnTypingStatusChanged called: " << isTyping << " " << peerId;
}

void UserController::OnConnectionFail(const QString& reason)
{
    qDebug() << " OnConnectionFail called : " << reason;
}

void UserController::CreateChat(const QString &contactName)
{
        chatList->AddChat(contactName);
}

ChatList *UserController::GetChatList() const
{
        return chatList;
}
DialogModel *UserController::GetDialogModel() const
{
    return dialogModel;
}

void UserController::SetUsername(const QString &name)
{
    if (name != username && !name.isEmpty()) {
        username = name;

        QSettings settings;
        settings.setValue("user/name", name);

        qDebug() << "User name set to:" << name;
        emit UsernameChanged();
        emit UsernameChangedForNetwork(name);
    }
}

void UserController::OnPeerConnected(const QString &peerID, const QString &peerAddress, const QString &peerName)
{
    qDebug() << "Peer connected - ID:" << peerID
            << "Name:" << peerName;

    QString displayName = !peerName.isEmpty() ? peerName :
                         "User_" + peerID.mid(1, 8);

    chatList->AddChat(peerID, true, displayName);

    emit ConnectionStatusChange(true);
}
