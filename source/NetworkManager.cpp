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


bool NetworkManager::StartP2PNode(quint16 preferredPort)
{
    if (isRunning) {
        return true;
    }

    currentPort = 0;
    isRunning = false;

    tcpServer = new QTcpServer(this);
    if (!tcpServer) {
        emit ServerStartFail();
        return false;
    }

    connect(tcpServer, &QTcpServer::newConnection,
            this, &NetworkManager::onNewIncomingConnection);

    if (preferredPort == 0) {
        if (!tcpServer->listen(QHostAddress::AnyIPv4, 0)) {
            tcpServer->deleteLater();
            tcpServer = nullptr;
            emit ServerStartFail();
            return false;
        }
        currentPort = tcpServer->serverPort();
    }
    else {
        bool listenSuccess = false;
        for (quint16 port = preferredPort; port < preferredPort + 10; port++) {
            if (tcpServer->listen(QHostAddress::AnyIPv4, port)) {
                currentPort = port;
                listenSuccess = true;
                break;
            }
        }

        if (!listenSuccess) {
            tcpServer->deleteLater();
            tcpServer = nullptr;
            emit ServerStartFail();
            return false;
        }
    }




    const quint16 UDP_SEND_PORT = 45000;    // port to send
    const quint16 UDP_RECV_PORT = 45001;    // port to receive

    udpSocket = new QUdpSocket(this);
    if (!udpSocket) {
        emit ServerStartFail();
        return false;
    }
    if (!udpSocket->bind(QHostAddress::AnyIPv4, UDP_SEND_PORT, QUdpSocket::ShareAddress)) {
        udpSocket->deleteLater();
        udpSocket = nullptr;
        emit ServerStartFail();
        return false;
    }

    const int BROADCAST_OPTION = 1;

    udpSocket->setSocketOption(static_cast<QAbstractSocket::SocketOption>(BROADCAST_OPTION), 1);

    QUdpSocket* broadcastListenSocket = new QUdpSocket(this);

    if (!broadcastListenSocket->bind(QHostAddress::AnyIPv4, UDP_RECV_PORT, QUdpSocket::ShareAddress)) {
        broadcastListenSocket->deleteLater();
    } else {
        connect(broadcastListenSocket, &QUdpSocket::readyRead,
                this, [this, broadcastListenSocket]() {
                    this->onBroadcastReceived(broadcastListenSocket);
                });

        m_broadcastListenSocket = broadcastListenSocket;
    }

    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this]() {
        if (!udpSocket) return;

        QJsonObject broadcastMsg;
        broadcastMsg["type"] = "discovery";
        broadcastMsg["tcp_port"] = currentPort;
        broadcastMsg["uuid"] = myUuid;
        broadcastMsg["user_name"] = username;

        QJsonDocument doc(broadcastMsg);
        QByteArray data = doc.toJson();

        // ВАЖНО: отправляем на ПОРТ ПРИЕМА (45001)
        QHostAddress broadcastAddr = QHostAddress::Broadcast;
        quint16 targetPort = 45001;

        qint64 sent = udpSocket->writeDatagram(data, broadcastAddr, targetPort);
    });
    timer->start(5000);

    isRunning = true;
    emit ServerStarted();

    return true;
}

void NetworkManager::onBroadcastReceived(QUdpSocket* socket)
{

    while (socket->hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(socket->pendingDatagramSize());

        QHostAddress senderAddress;
        quint16 senderPort;

        qint64 bytesRead = socket->readDatagram(datagram.data(), datagram.size(),
                                                &senderAddress, &senderPort);

        QJsonDocument doc = QJsonDocument::fromJson(datagram);
        if (doc.isNull()) continue;

        QJsonObject json = doc.object();
        QString type = json["type"].toString();

        if (type == "discovery")
        {
            QString peerUuid = json["uuid"].toString();
            quint16 peerTcpPort = json["tcp_port"].toInt();
            QString peerName = json["user_name"].toString();

            if (peerUuid == myUuid) continue;

            bool alreadyConnected = false;
            for (const QString& connectedPeerId : activePeers.keys()) {
                if (connectedPeerId == peerUuid) {
                    alreadyConnected = true;
                    break;
                }
            }

            if (!alreadyConnected && isRunning) {
                ConnectToPeer(senderAddress.toString(), peerTcpPort);
            }
        }
    }
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
    broadcastMsg["user_name"] = username;

    QJsonDocument doc(broadcastMsg);
    QByteArray data = doc.toJson();

    udpSocket->writeDatagram(data, QHostAddress::Broadcast, currentPort);

    QHostAddress broadcastAddr = QHostAddress::Broadcast;
    quint16 targetPort = udpSocket->localPort();
    qint64 sent = udpSocket->writeDatagram(data, broadcastAddr, targetPort);
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


        if (!peerUuid.isEmpty() && !peerId.startsWith("temp_")) {
            QTcpSocket* socket = activePeers.take(peerId);
            activePeers.insert(peerUuid, socket);

            emit PeerConnected(peerUuid, peerName);
        }
    }
    else if (type == "text") {


        QString message = json["message"].toString();
        QString from = json["from"].toString();
        QDateTime timestamp = QDateTime::fromString(json["timestamp"].toString(), Qt::ISODate);

        emit MessageReceived(from, message, timestamp);
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

    emit PeerConnected(tempPeerId, socket->peerName());
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



void NetworkManager::SendMessage(MessageClass* message, const QString &userAddress)
{
    if (!activePeers.contains(userAddress)) return;
    QTcpSocket* socket = activePeers.value(userAddress);

    QJsonObject msg;
    msg["type"] = "text";
    msg["from"] = myUuid;
    msg["message"] = message->Get_content();
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
    emit PeerConnected(tempPeerId, username);
}

void NetworkManager::SendJsonToSocket(QTcpSocket* socket, const QJsonObject& json)
{
    if (!socket || !socket->isOpen()) return;

    QJsonDocument doc(json);
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);

    quint32 messageSize = jsonData.size();
    QByteArray sizeData;
    QDataStream sizeStream(&sizeData, QIODevice::WriteOnly);
    sizeStream.setByteOrder(QDataStream::BigEndian);
    sizeStream << messageSize;

    QByteArray packet = sizeData + jsonData;

    qint64 bytesWritten = socket->write(sizeData + jsonData);

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
    if (!udpSocket) return;


    static bool firstTime = true;
    if (firstTime) {
        firstTime = false;
    }

    if (!udpSocket) return;

    while (udpSocket->hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());

        QHostAddress senderAddress;
        quint16 senderPort;


        qint64 bytesRead = udpSocket->readDatagram(datagram.data(), datagram.size(), &senderAddress, &senderPort);


        if (bytesRead <= 0) continue;
        QJsonDocument doc = QJsonDocument::fromJson(datagram);
        if (doc.isNull())
        {
            continue;
        }
        QJsonObject json = doc.object();

        QString type = json["type"].toString();

        if (type == "discovery")
        {
            QString peerUuid = json["uuid"].toString();
            quint16 peerTcpPort = json["tcp_port"].toInt();
            QString peerName = json["user_name"].toString();


            if (peerUuid == myUuid) {
                continue;
            }

            bool alreadyConnected = false;
            for (const QString& connectedPeerId : activePeers.keys()) {
                if (connectedPeerId == peerUuid) {
                    alreadyConnected = true;
                    break;
                }
                if (connectedPeerId.contains(senderAddress.toString())) {
                    alreadyConnected = true;
                    break;
                }
            }
            if (!alreadyConnected && isRunning)
            {
                ConnectToPeer(senderAddress.toString(), peerTcpPort);
            }
        }
    }
}

void NetworkManager::SetUserInfo(const QString &uuid, const QString &username)
{
    this->myUuid = uuid;
    this->username = username;
}

void NetworkManager::SetUuid(QString uuid)
{
    myUuid = uuid;
}

void NetworkManager::SetUsername(QString username)
{
    this->username = username;
}

