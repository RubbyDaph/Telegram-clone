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

    connect(this, &UserController::DialogModelChanged, dialogModel, &DialogModel::OnDialogModelChanged);
}


void UserController::SetCurrentChat(const int &chatID)
{
    dialogModel = chatList->getDialogModel(chatID);

    emit DialogModelChanged(dialogModel);
}



void UserController::SendMessage(const QString &message)
{
    if (!dialogModel || message.trimmed().isEmpty()) return;

    // СОЗДАЕМ НОВЫЙ объект для каждого сообщения
    MessageClass* newMessage = new MessageClass(
        "You",  // senderName
        message, // content
        QDateTime::currentDateTime().toString("hh:mm"), // timestamp
        true     // isOwnMessage
    );

    dialogModel->AddMessage(newMessage);
}

void UserController::StartMessaging()
{
    qDebug() << "StartMessaging called";
}

void UserController::Logout(const QString &userAddress)
{
    qDebug() << "Logout called " << userAddress;
}

void UserController::OnMessageReceived(const QString &receivedMessage, const QString &sender)
{
    qDebug() << "OnMessageReceived called  text: " << receivedMessage << " From " << sender;
}

void UserController::OnMessageSent(const QString &sentMessage, const QString &messageID, bool success)
{
    qDebug() << "OnMessageSent called text: " << sentMessage << " " << messageID << " " << success;
}

void UserController::OnConnectionStatusChanged(bool isOnline)
{
    qDebug() << "OnConnectionStatusChanged called: " << isOnline;
}

void UserController::OnTypingStatusChanged(bool isTyping, const QString &userAddress)
{
    qDebug() << "OnTypingStatusChanged called: " << isTyping << " " << userAddress;
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



