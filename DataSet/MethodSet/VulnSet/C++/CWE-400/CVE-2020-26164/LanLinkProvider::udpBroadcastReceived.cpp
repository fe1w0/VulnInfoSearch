void LanLinkProvider::udpBroadcastReceived()
{
    while (m_udpSocket.hasPendingDatagrams()) {

        QByteArray datagram;
        datagram.resize(m_udpSocket.pendingDatagramSize());
        QHostAddress sender;

        m_udpSocket.readDatagram(datagram.data(), datagram.size(), &sender);

        if (sender.isLoopback() && !m_testMode)
            continue;

        NetworkPacket* receivedPacket = new NetworkPacket(QLatin1String(""));
        bool success = NetworkPacket::unserialize(datagram, receivedPacket);

        //qCDebug(KDECONNECT_CORE) << "udp connection from " << receivedPacket->;

        //qCDebug(KDECONNECT_CORE) << "Datagram " << datagram.data() ;

        if (!success) {
            qCDebug(KDECONNECT_CORE) << "Could not unserialize UDP packet";
            delete receivedPacket;
            continue;
        }

        if (receivedPacket->type() != PACKET_TYPE_IDENTITY) {
            qCDebug(KDECONNECT_CORE) << "Received a UDP packet of wrong type" << receivedPacket->type();
            delete receivedPacket;
            continue;
        }

        if (receivedPacket->get<QString>(QStringLiteral("deviceId")) == KdeConnectConfig::instance().deviceId()) {
            //qCDebug(KDECONNECT_CORE) << "Ignoring my own broadcast";
            delete receivedPacket;
            continue;
        }

        int tcpPort = receivedPacket->get<int>(QStringLiteral("tcpPort"));

        //qCDebug(KDECONNECT_CORE) << "Received Udp identity packet from" << sender << " asking for a tcp connection on port " << tcpPort;

        if (m_receivedIdentityPackets.size() > MAX_REMEMBERED_IDENTITY_PACKETS) {
            qCWarning(KDECONNECT_CORE) << "Too many remembered identities, ignoring" << receivedPacket->get<QString>(QStringLiteral("deviceId")) << "received via UDP";
            delete receivedPacket;
            continue;
        }

        QSslSocket* socket = new QSslSocket(this);
        socket->setProxy(QNetworkProxy::NoProxy);
        m_receivedIdentityPackets[socket].np = receivedPacket;
        m_receivedIdentityPackets[socket].sender = sender;
        connect(socket, &QAbstractSocket::connected, this, &LanLinkProvider::tcpSocketConnected);
#if QT_VERSION < QT_VERSION_CHECK(5,15,0)
        connect(socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, &LanLinkProvider::connectError);
#else
        connect(socket, &QAbstractSocket::errorOccurred, this, &LanLinkProvider::connectError);
#endif
        socket->connectToHost(sender, tcpPort);
    }
}