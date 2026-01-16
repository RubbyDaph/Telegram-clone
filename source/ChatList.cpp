#include "ChatList.h"


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
            return chat.peerUuid;
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

int ChatList::findChatById(QString uuid)
{
    for (int i = 0; i < m_chats.size(); ++i) {
        if (m_chats[i].peerUuid == uuid) {
            return i;
        }
    }
    return -1;
}

void ChatList::AddChat(const QString &identifier, bool isUuid, const QString& name)
{
    QString uuid;
    QString displayName;

    if (isUuid)
    {
        uuid = identifier;

        if (!name.isEmpty())
        {
            displayName = name;
        }
        else
        {
            if (uuid.startsWith("temp_"))
            {
                displayName = "User (" + uuid.mid(5) + ")";
            }
            else if (uuid.startsWith("{"))
            {
                displayName = "User_" + uuid.mid(1, 8);
            }
            else
            {
                displayName = uuid;
            }
        }
    }
    else
    {
        displayName = identifier;
        uuid = "local_" + QUuid::createUuid().toString();
    }

    if (findChatById(uuid) != -1)
    {
        return;
    }

    beginInsertRows(QModelIndex(), m_chats.size(), m_chats.size());
    m_chats.append(ChatItem(displayName, uuid));
    endInsertRows();
}

DialogModel* ChatList::getDialogModel(QString chatId)
{

    for (int i = 0; i < m_chats.size(); ++i) {
        if (m_chats[i].peerUuid == chatId) {
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

QString ChatList::getContactNameByID(QString chatId)
{

    for (int i = 0; i < m_chats.size(); ++i) {
        if (m_chats[i].peerUuid == chatId) {
            return m_chats[i].contactName;
        }
    }
    return "";
}
