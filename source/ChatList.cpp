#include "ChatList.h"

quint64 ChatList::nextId = 1;


int ChatList::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_chats.count();
}



QVariant ChatList::data(const QModelIndex &index, int role) const
{


    if (!index.isValid() || index.row() < 0 || index.row() >= m_chats.size())
        return QVariant();

    const ChatItem &chat = m_chats.at(index.row());

    switch (role) {
        case ContactNameRole:
            return chat.contactName;
        case LastMessageRole:
            return chat.lastMessage;
        case TimestampRole:
            return chat.timestamp;
        case ChatId:
            return chat.id;
        default:
            return QVariant();
    }
}

QHash<int, QByteArray> ChatList::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[ContactNameRole] = "contactName";
    roles[LastMessageRole] = "lastMessage";
    roles[TimestampRole]   = "timestamp";
    roles[ChatId]          = "chatId";
    return roles;
}

void ChatList::AddChat(const QString &contactName)
{

    beginInsertRows(QModelIndex(), m_chats.size(), m_chats.size());
    m_chats.append(ChatItem(contactName));
    endInsertRows();
}
DialogModel* ChatList::getDialogModel(quint64 chatId)
{

    for (int i = 0; i < m_chats.size(); ++i) {
        if (m_chats[i].id == chatId) {
            return m_chats[i].dialogModel;
        }
    }
    return nullptr;
}
void ChatList::updateLastMessage(int chatIndex, const QString &message, const QString &sender)
{
    if (chatIndex >= 0 && chatIndex < m_chats.size()) {
        ChatItem &chat = m_chats[chatIndex];
        chat.lastMessage = message;
        chat.timestamp = QDateTime::currentDateTime().toString("hh:mm");

        QModelIndex modelIndex = createIndex(chatIndex, 0);
        emit dataChanged(modelIndex, modelIndex, {LastMessageRole, TimestampRole});
    }
}
int ChatList::findChatByContactName(const QString &contactName)
{
    for (int i = 0; i < m_chats.size(); ++i) {
        if (m_chats[i].contactName == contactName) {
            return i;
        }
    }
    return -1;
}

QString ChatList::getContactNameByID(quint64 chatId)
{

    for (int i = 0; i < m_chats.size(); ++i) {
        if (m_chats[i].id == chatId) {
            return m_chats[i].contactName;
        }
    }
    return "";
}
