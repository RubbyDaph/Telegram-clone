#pragma once

#include "DialogClass.h"
#include <QDateTime>
#include <QObject>

class ChatList : public QAbstractListModel
{
    Q_OBJECT
    static quint64 nextId;
    struct ChatItem {
        QString contactName;
        QString lastMessage;
        QString timestamp;
        quint64 id;
        DialogModel* dialogModel;

    ChatItem(const QString &name)
            : contactName(name)
            , lastMessage("Нет сообщений")
            , timestamp("")
            , id(nextId++)
            , dialogModel(new DialogModel())
    {};
};
    QList<ChatItem> m_chats;

public:
    explicit ChatList(QObject* parent = nullptr) : QAbstractListModel(parent) {}
    int rowCount(const QModelIndex & parent) const override ;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
    Q_INVOKABLE DialogModel* getDialogModel(quint64 chatId);
    Q_INVOKABLE void updateLastMessage(int chatIndex, const QString &message, const QString &sender);
    Q_INVOKABLE int findChatByContactName(const QString &contactName);
public slots:
    void AddChat(const QString& contactName);
public:
    enum Roles
    {
        ContactNameRole = Qt::UserRole + 1,
        LastMessageRole,
        TimestampRole,
        ChatId
    };
};