void LanLinkProvider::dataReceived()
{
    QSslSocket* socket = qobject_cast<QSslSocket*>(sender());
#if QT_VERSION < QT_VERSION_CHECK(5,7,0)
    if (!socket->canReadLine())
        return;
#else
    socket->startTransaction();
#endif

    const QByteArray data = socket->readLine();

    qCDebug(KDECONNECT_CORE) << "LanLinkProvider received reply:" << data;

    NetworkPacket* np = new NetworkPacket(QLatin1String(""));
    bool success = NetworkPacket::unserialize(data, np);

#if QT_VERSION < QT_VERSION_CHECK(5,7,0)
    if (!success) {
        delete np;
        return;
    }
#else
    if (!success) {
        delete np;
        socket->rollbackTransaction();
        return;
    }
    socket->commitTransaction();
#endif

    if (np->type() != PACKET_TYPE_IDENTITY) {
        qCWarning(KDECONNECT_CORE) << "LanLinkProvider/newConnection: Expected identity, received " << np->type();
        delete np;
        return;
    }

    // Needed in "encrypted" if ssl is used, similar to "tcpSocketConnected"
    m_receivedIdentityPackets[socket].np = np;

    const QString& deviceId = np->get<QString>(QStringLiteral("deviceId"));
    //qCDebug(KDECONNECT_CORE) << "Handshaking done (i'm the new device)";

    //This socket will now be owned by the LanDeviceLink or we don't want more data to be received, forget about it
    disconnect(socket, &QIODevice::readyRead, this, &LanLinkProvider::dataReceived);

    if (np->get<int>(QStringLiteral("protocolVersion")) >= MIN_VERSION_WITH_SSL_SUPPORT) {

        bool isDeviceTrusted = KdeConnectConfig::instance().trustedDevices().contains(deviceId);
        configureSslSocket(socket, deviceId, isDeviceTrusted);

        qCDebug(KDECONNECT_CORE) << "Starting client ssl (but I'm the server TCP socket)";

        connect(socket, &QSslSocket::encrypted, this, &LanLinkProvider::encrypted);

        if (isDeviceTrusted) {
            connect(socket, QOverload<const QList<QSslError> &>::of(&QSslSocket::sslErrors), this, &LanLinkProvider::sslErrors);
        }

        socket->startClientEncryption();

    } else {
        qWarning() << np->get<QString>(QStringLiteral("deviceName")) << "uses an old protocol version, this won't work";
        //addLink(deviceId, socket, np, LanDeviceLink::Locally);
        delete m_receivedIdentityPackets.take(socket).np;
    }
}