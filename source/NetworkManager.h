#pragma once

#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>
#include <QUdpSocket>
#include <QHash>
#include <QJsonDocument>
#include <QTimer>
#include <QJsonObject>
#include <QAbstractSocket>
#include "MessageClass.h"

class NetworkManager : public QObject
{
    Q_OBJECT
public:
    explicit NetworkManager(QObject* parent);
    ~NetworkManager();
    void ConnectToPeer(const QString& userAddress, quint16 port);
    void DisconnectFromPeer(const QString& peerId);

    bool StartP2PNode(quint16 preferredPort);
    void StopP2PNode();

    void SendMessage(MessageClass* message, const QString& userAddress);
    void sendPresenceStatus(const QString& peerId, bool isOnline);
    QString GetMyUuid() const {return myUuid;}
    bool IsRunning() const {return isRunning;}
    void SetUserInfo(const QString& uuid, const QString& username);
    void SetUuid(QString uuid);
    void SetUsername(QString username);

    Q_INVOKABLE void debugPeers() {
        qDebug() << "Active peers:" << activePeers.keys();
    }

signals:
    void ServerStarted();
    void ServerStartFail();

    void peerPresenceChanged(const QString& peerId, bool isOnline);

    void PeerConnected(const QString& peerID, const QString& peerName = "");
    void PeerDisconnected(const QString& peerID, const QString& peerAddress);

    void MessageReceived(const QString& peerAddress, const QString& message, const QDateTime& timestamp);
private slots:
    void onBroadcastReceived(QUdpSocket* socket);
    void onNewIncomingConnection();
    void onPeerReadyRead();
    void onUdpDataReceived();
    void onPeerDisconnected(QTcpSocket* socket);
    void onPeerConnected(QTcpSocket* socket);
public slots:
    void onUsernameChanged(const QString& newName);
private:
    QUdpSocket* m_broadcastListenSocket = nullptr;
    QTcpServer* tcpServer;

    QHash<QString, QTcpSocket*> activePeers;
    QUdpSocket* udpSocket;

    QString username;
    quint16 currentPort;
    QString myUuid;
    bool isRunning;

    void SetupServer(quint16 port);
    void SetupUdpDiscovery(quint16 port);
    void BroadcastPresence();
    void ProcessNetworkMessage(QTcpSocket* senderSocket, const QByteArray& data);
    void sendStatusPacket(const QString& peerId, bool status, const QString& type);
    void SendJsonToSocket(QTcpSocket* socket, const QJsonObject& json);
    void SendHelloMessage(QTcpSocket* socket);;
};