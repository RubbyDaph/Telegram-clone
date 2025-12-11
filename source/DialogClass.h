#pragma once

#include <QList>
#include <QAbstractListModel>
#include "MessageClass.h"

class DialogModel : public QAbstractListModel // it's the child class in order
{//                             to connect QML and c++ code, to be able to change the listview content
    Q_OBJECT
private:
    QList<MessageClass*> m_messages;
    QString m_contactName;
public:
    explicit DialogModel(QObject* parent = nullptr) : QAbstractListModel(parent) {}
    int rowCount(const QModelIndex & parent) const override ;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
    void AddMessage(MessageClass *message);

    enum Roles
    {
        SenderRole = Qt::UserRole + 1,
        ContentRole,
        TimestampRole,
        IsOwnMessageRole
    };
    public slots:
    void OnDialogModelChanged(DialogModel* dialogModel);
};