#include "DialogClass.h"

QHash<int, QByteArray> DialogModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[SenderRole]       = "senderName";
    roles[ContentRole]      = "content";
    roles[TimestampRole]   = "timestamp";
    roles[IsOwnMessageRole] = "isOwnMessage";
    return roles;
}

int DialogModel::rowCount(const QModelIndex & parent) const
{
    Q_UNUSED(parent)
    return m_messages.count();
}
QVariant DialogModel::data(const QModelIndex &index, int role) const
{

    if (!index.isValid() || index.row() < 0 || index.row() >= m_messages.size())
        return QVariant();

    MessageClass* message = m_messages.at(index.row());

    switch (role) {
        case SenderRole:
            return message->Get_senderName();
        case ContentRole:
            return message->Get_content();
        case TimestampRole:
            return message->Get_timestamp();
        case IsOwnMessageRole:
            return message->Get_isOwnMessage();
        default:
            return QVariant();
    }
}
void DialogModel::AddMessage(MessageClass *message)
{
    beginInsertRows(QModelIndex(), m_messages.size(), m_messages.size());
    m_messages.append(message);
    endInsertRows();
}

void DialogModel::OnDialogModelChanged(DialogModel *dialogModel)
{
    if (this == dialogModel) return; // Это тот же объект

    // Очищаем текущие сообщения
    if (!m_messages.empty()) {
        beginRemoveRows(QModelIndex(), 0, m_messages.size() - 1);
        m_messages.clear();
        endRemoveRows();
    }

    // Добавляем сообщения из нового диалога
    if (!dialogModel->m_messages.empty()) {
        beginInsertRows(QModelIndex(), 0, dialogModel->m_messages.size() - 1);
        for (auto* message : dialogModel->m_messages) {
            m_messages.append(message);
        }
        endInsertRows();
    }
}


