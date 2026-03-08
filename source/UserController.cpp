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

    QSettings settings;
    username = settings.value("user/name", "User").toString();
    QString savedUuid = settings.value("user/uuid", "").toString();

    if (!savedUuid.isEmpty())
    {
        networkManager->SetUuid(savedUuid);
    }

    myUuid = networkManager->GetMyUuid();

    if (savedUuid.isEmpty() && !myUuid.isEmpty())
    {
        settings.setValue("user/uuid", myUuid);
    }

    if (!username.isEmpty())
    {
        networkManager->SetUsername(username);
        emit UsernameChangedForNetwork(username);
    }



    connect(networkManager, &NetworkManager::MessageReceived, this, &UserController::OnMessageReceived);
    connect(networkManager, &NetworkManager::peerPresenceChanged, this, &UserController::onPeerPresenceChanged);
    connect(networkManager, &NetworkManager::PeerConnected, this, &UserController::OnPeerConnected);
    connect(networkManager, &NetworkManager::PeerDisconnected, this, &UserController::OnPeerDisconnected);
    connect(this, &UserController::DialogModelChanged, dialogModel, &DialogModel::OnDialogModelChanged);
    connect(this, &UserController::UsernameChangedForNetwork, networkManager, &NetworkManager::onUsernameChanged);
}


void UserController::SetCurrentChat(const QString &chatID)
{
    currentChatId = chatID;
    dialogModel = chatList->getDialogModel(chatID);

    emit DialogModelChanged(dialogModel);
}

void UserController::StartMessaging()
{

    bool started = networkManager->StartP2PNode(0);

    if (!started) {
        emit ConnectionFail("Failed to start P2P node - no available ports");
    }
}

void UserController::Login(const QString& username)
{
    SetUsername(username);

    if (myUuid.isEmpty())
    {
        myUuid = QUuid::createUuid().toString();
    }

    StartMessaging();
}

void UserController::Logout()
{

    networkManager->StopP2PNode();
    username.clear();
    myUuid.clear();

    QSettings settings;
    settings.remove("user/name");
    settings.remove("user/uuid");

    emit ConnectionStatusChange(false);
}

bool UserController::IsLoggedIn() const
{
    return !username.isEmpty() && networkManager->IsRunning();
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
    networkManager->SendMessage(newMessage, GetCurrentChatId());
    QString messageId = QUuid::createUuid().toString();

    emit MessageSent(message, messageId, true);
}


void UserController::OnMessageReceived(const QString& peerUuid, const QString& message, const QDateTime& timestamp)
{

    int chatIndex = chatList->findChatById(peerUuid);

    DialogModel* targetDialogModel = chatList->getDialogModel(peerUuid);


    QString senderName = chatList->getContactNameByID(peerUuid);

    MessageClass* receivedMessage = new MessageClass(
        senderName,
        message,
        timestamp.toString("hh:mm"),
        false
    );

        targetDialogModel->AddMessage(receivedMessage);
        chatList->updateLastMessage(chatIndex, message, senderName);


    if (dialogModel == targetDialogModel) emit DialogModelChanged(dialogModel);
}

void UserController::onPeerPresenceChanged(const QString& peerId, bool isOnline)
{
    qDebug() << "OnConnectionStatusChanged called: " << isOnline;
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

        emit UsernameChanged();
        emit UsernameChangedForNetwork(name);
    }
}

void UserController::OnPeerConnected(const QString &peerID, const QString &peerName)
{

    if (peerID.startsWith("temp_")) return;

    int chatIndex = chatList->findChatById(peerID);

    if (chatIndex == -1) return;

    chatList->AddChat(peerID, true, peerName);


    emit ConnectionStatusChange(true);
}

void UserController::OnPeerDisconnected(const QString &peerID, const QString &peerAddress)
{

}

QString UserController::GetCurrentUsername() const
{
    return username;
}

QString UserController::GetCurrentChatId() const
{
    return currentChatId;
}
