#include "NetworkManager.h"
#include <QDebug>

NetworkManager::NetworkManager(QObject* parent)
    : QObject(parent)
    , tcpServer(nullptr)
    , udpSocket(nullptr)
    , username("User")
    , isRunning(false)
    , currentPort(0)
{
    myUuid = QUuid::createUuid().toString();
}
NetworkManager::~NetworkManager()
{
    StopP2PNode();
    if (!activePeers.isEmpty())
    {
        for (QTcpSocket* socket : activePeers) {
            if (socket)
            {
                socket->disconnectFromHost();
                socket->deleteLater();
            }
        }
        activePeers.clear();
    }
    if (tcpServer)
    {
        tcpServer->deleteLater();
        tcpServer = nullptr;
    }
    if (udpSocket)
    {
        udpSocket->deleteLater();
        udpSocket = nullptr;
    }
}

void NetworkManager::ConnectToPeer(const QString &userAddress, quint16 port)
{
    QTcpSocket* socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::connected, this,
            [this, socket]() { this->onPeerConnected(socket); });
    connect(socket, &QTcpSocket::disconnected, this,
            [this, socket]() { this->onPeerDisconnected(socket); });
    connect(socket, &QTcpSocket::readyRead, this, &NetworkManager::onPeerReadyRead);

    socket->connectToHost(userAddress, port);
}


bool NetworkManager::StartP2PNode(quint16 preferredPort = 0)
{

    if (preferredPort == 0) {
        preferredPort = 49152;
    }

    for (quint16 port = preferredPort; port < 65535; port++) {
        if (tcpServer->listen(QHostAddress::Any, port)) {
            currentPort = port;
            qDebug() << "Server started on port:" << port;
            return true;
        }
    }

    return false;
}

void NetworkManager::StopP2PNode()
{
    for (QTcpSocket* socket : activePeers)
    {
        socket->disconnectFromHost();
        socket->deleteLater();
    }
    activePeers.clear();

    if (tcpServer && tcpServer->isListening())
    {
        tcpServer->close();
    }
    if (udpSocket && udpSocket->isOpen())
    {
        udpSocket->close();
    }

}

void NetworkManager::SetupUdpDiscovery(quint16 port)
{
    udpSocket = new QUdpSocket(this);
    if (udpSocket->bind(port, QUdpSocket::ShareAddress))
    {
        connect(udpSocket, &QUdpSocket::readyRead, this, &NetworkManager::onUdpDataReceived);

        QTimer* timer = new QTimer(this);
        connect(timer , &QTimer::timeout, this , &NetworkManager::BroadcastPresence);
        timer->start(5000);
    }
}

void NetworkManager::BroadcastPresence()
{
    if (!udpSocket) return;

    QJsonObject broadcastMsg;
    broadcastMsg["type"] = "discovery";
    broadcastMsg["tcp_port"] = currentPort;
    broadcastMsg["uuid"] = myUuid;

    QJsonDocument doc(broadcastMsg);
    QByteArray data = doc.toJson();

    udpSocket->writeDatagram(data, QHostAddress::Broadcast, currentPort);
}

void NetworkManager::ProcessNetworkMessage(QTcpSocket* senderSocket, const QByteArray& data)
{
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull()) return;

    QJsonObject json = doc.object();
    QString type = json["type"].toString();

    QString peerId;
    for (auto it = activePeers.begin(); it != activePeers.end(); ++it) {
        if (it.value() == senderSocket) {
            peerId = it.key();
            break;
        }
    }

    if (type == "hello") {
        QString peerUuid = json["uuid"].toString();
        QString peerName = json["user_name"].toString();
        if (!peerUuid.isEmpty() && peerId.startsWith("temp_")) {
            QTcpSocket* socket = activePeers.take(peerId);
            activePeers.insert(peerUuid, socket);

            QString peerAddress = socket->peerAddress().toString() + ":" + QString::number(socket->peerPort());

            emit PeerConnected(peerUuid, peerAddress, peerName);
        }
    }
    else if (type == "text") {
        QString message = json["message"].toString();
        QDateTime timestamp = QDateTime::fromString(json["timestamp"].toString(), Qt::ISODate);
        emit MessageReceived(peerId, message, timestamp);
    }
    else if (type == "typing") {
        bool isTyping = json["status"].toBool();
        emit peerTypingStatusChanged(peerId, isTyping);
    }
    else if (type == "presence") {
        bool isOnline = json["status"].toBool();
        emit peerPresenceChanged(peerId, isOnline);
    }
}

void NetworkManager::sendStatusPacket(const QString& peerId, bool status, const QString& type)
{
    if (!activePeers.contains(peerId)) return;

    QJsonObject msg;
    msg["type"] = type;
    msg["status"] = status;
    msg["from"] = myUuid;
    msg["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);

    QTcpSocket* socket = activePeers.value(peerId);
    SendJsonToSocket(socket, msg);
}

void NetworkManager::onPeerReadyRead()
{
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;

    while (true) {
        if (socket->bytesAvailable() < 4) return;

        QByteArray sizeData = socket->peek(4);
        QDataStream sizeStream(sizeData);
        sizeStream.setByteOrder(QDataStream::BigEndian);
        quint32 messageSize;
        sizeStream >> messageSize;

        if (socket->bytesAvailable() < 4 + messageSize) return;

        socket->read(4);
        QByteArray messageData = socket->read(messageSize);

        ProcessNetworkMessage(socket, messageData);
    }
}


void NetworkManager::sendTypingStatus(const QString& peerId, bool isTyping)
{
    sendStatusPacket(peerId, isTyping, "typing");
}

void NetworkManager::sendPresenceStatus(const QString& peerId, bool isOnline)
{
    sendStatusPacket(peerId, isOnline, "presence");
}

void NetworkManager::DisconnectFromPeer(const QString& peerId)
{
    if (activePeers.contains(peerId)) {
        QTcpSocket* socket = activePeers.value(peerId);
        socket->disconnectFromHost();
        activePeers.remove(peerId);
        socket->deleteLater();

        QString peerAddress = socket->peerAddress().toString() + ":" +
                             QString::number(socket->peerPort());
        emit PeerDisconnected(peerId, peerAddress);
    }
}

void NetworkManager::onPeerConnected(QTcpSocket* socket)
{
    QString tempPeerId = QString("temp_%1:%2")
                         .arg(socket->peerAddress().toString())
                         .arg(socket->peerPort());

    if (activePeers.contains(tempPeerId)) {
        socket->deleteLater();
        return;
    }

    activePeers.insert(tempPeerId, socket);

    QJsonObject helloMsg;
    helloMsg["type"] = "hello";
    helloMsg["uuid"] = myUuid;
    helloMsg["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    helloMsg["user_name"] = username;

    QJsonDocument doc(helloMsg);
    QByteArray jsonData = doc.toJson();
    quint32 size = jsonData.size();
    QByteArray sizeData;
    QDataStream sizeStream(&sizeData, QIODevice::WriteOnly);
    sizeStream.setByteOrder(QDataStream::BigEndian);
    sizeStream << size;

    if (socket->isOpen()) {
        socket->write(sizeData + jsonData);
    }

    emit PeerConnected(tempPeerId, socket->peerAddress().toString());
}

void NetworkManager::onPeerDisconnected(QTcpSocket* socket)
{
    QString peerId;
    QString peerAddress;

    for (auto it = activePeers.begin(); it != activePeers.end(); ++it) {
        if (it.value() == socket) {
            peerId = it.key();
            peerAddress = socket->peerAddress().toString() + ":" +
                         QString::number(socket->peerPort());
            break;
        }
    }

    if (!peerId.isEmpty()) {
        activePeers.remove(peerId);
        emit PeerDisconnected(peerId, peerAddress);
    }

    socket->deleteLater();
}



void NetworkManager::SendMessage(const QString &message, const QString &userAddress)
{
    if (!activePeers.contains(userAddress)) {
        qDebug() << "Peer not found:" << userAddress;
        return;
    }

    QTcpSocket* socket = activePeers.value(userAddress);

    QJsonObject msg;
    msg["type"] = "text";
    msg["from"] = myUuid;
    msg["message"] = message;
    msg["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);

    SendJsonToSocket(socket, msg);
}




void NetworkManager::onNewIncomingConnection()
{
    QTcpSocket* newSocket = tcpServer->nextPendingConnection();
    QString tempPeerId = QString("temp_%1:%2")
                        .arg(newSocket->peerAddress().toString())
                        .arg(newSocket->peerPort());

    if (activePeers.contains(tempPeerId))
    {
        newSocket->close();
        newSocket->deleteLater();
        return;
    }

    activePeers.insert(tempPeerId, newSocket);
    connect(newSocket, &QTcpSocket::readyRead, this, &NetworkManager::onPeerReadyRead);
    connect(newSocket, &QTcpSocket::disconnected, this,
            [this, newSocket]() { this->onPeerDisconnected(newSocket); });

    newSocket->setParent(this);

    QJsonObject helloMsg;
    helloMsg["type"] = "hello";
    helloMsg["uuid"] = myUuid;
    helloMsg["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    helloMsg["user_name"] = username;

    SendJsonToSocket(newSocket, helloMsg);

    QString peerAddress = newSocket->peerAddress().toString() + ":" +
                          QString::number(newSocket->peerPort());
    emit PeerConnected(tempPeerId, peerAddress);
}

void NetworkManager::SendJsonToSocket(QTcpSocket* socket, const QJsonObject& json)
{
    if (!socket || !socket->isOpen())
    {
        qDebug() << "Socket is not open or invalid";
        return;
    }

    QJsonDocument doc(json);
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);

    // Исправлено: кросс-платформенная запись размера
    quint32 messageSize = jsonData.size();
    QByteArray sizeData;
    QDataStream sizeStream(&sizeData, QIODevice::WriteOnly);
    sizeStream.setByteOrder(QDataStream::BigEndian);
    sizeStream << messageSize;

    qint64 bytesWritten = socket->write(sizeData + jsonData);

    if (bytesWritten == -1)
    {
        qDebug() << "Failed to send message:" << socket->errorString();
    }
}

void NetworkManager::onUsernameChanged(const QString& newName)
{
    if (newName != username)
    {
        username = newName;
        // TODO : позже добавить рассылку для всех пиров
    }
}

void NetworkManager::SendHelloMessage(QTcpSocket* socket)
{
    QJsonObject helloMsg;
    helloMsg["type"] = "hello";
    helloMsg["uuid"] = myUuid;
    helloMsg["user_name"] = username;
    helloMsg["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);

    SendJsonToSocket(socket, helloMsg);

}

void NetworkManager::onUdpDataReceived()
{

}