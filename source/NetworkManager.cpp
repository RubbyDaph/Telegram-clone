#include "NetworkManager.h"
#include <QDebug>

void NetworkManager::ConnectToHost(const QString &userAddress, quint16 port)
{
    qDebug() << "ConnectToHost called with " << userAddress << " " << port;
}

void NetworkManager::DisconnectUser(const QString &userAddress, quint16 port)
{
    qDebug() << "DisconnetUser called " << userAddress << " " << port;
}

void NetworkManager::StartServer(quint16 port)
{
    qDebug() << "StartServer called " << port;
}

void NetworkManager::SendMessage(const QString &message, const QString &userAddress)
{
    qDebug() << "SendMessage " << message << " " << userAddress;
}




