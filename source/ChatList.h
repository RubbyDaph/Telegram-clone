#pragma once

#include "DialogClass.h"
#include <QDateTime>
#include <QObject>
#include <QUuid>

class ChatList : public QAbstractListModel
{
    Q_OBJECT
    struct ChatItem {
        QString contactName;
        QString lastMessage;
        QString timestamp;
        QString peerUuid;
        DialogModel* dialogModel;

    ChatItem(const QString &name, const QString& uuid)
            : contactName(name)
            , lastMessage("Нет сообщений")
            , timestamp("")
            , peerUuid(uuid)
            , dialogModel(new DialogModel())
    {};
    };
    QList<ChatItem> m_chats;

public:
    explicit ChatList(QObject* parent = nullptr) : QAbstractListModel(parent) {}
    int rowCount(const QModelIndex & parent) const override ;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
    Q_INVOKABLE DialogModel* getDialogModel(const QString& chatId);
    Q_INVOKABLE void updateLastMessage(int chatIndex, const QString &message, const QString &sender);
    Q_INVOKABLE int findChatById(QString uuid);
    Q_INVOKABLE QString getContactNameByID(QString chatId);
public slots:
    void AddChat(const QString &identifier, bool isUuid = false, const QString& name = "");
public:
    enum Roles
    {
        ContactNameRole = Qt::UserRole + 1,
        LastMessageRole,
        TimestampRole,
        ChatId
    };
};