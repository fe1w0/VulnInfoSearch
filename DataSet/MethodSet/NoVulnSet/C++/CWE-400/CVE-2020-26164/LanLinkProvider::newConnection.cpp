void LanLinkProvider::newConnection()
{
    qCDebug(KDECONNECT_CORE) << "LanLinkProvider newConnection";

    while (m_server->hasPendingConnections()) {
        QSslSocket* socket = m_server->nextPendingConnection();
        configureSocket(socket);
        //This socket is still managed by us (and child of the QTcpServer), if
        //it disconnects before we manage to pass it to a LanDeviceLink, it's
        //our responsibility to delete it. We do so with this connection.
        connect(socket, &QAbstractSocket::disconnected,
                socket, &QObject::deleteLater);
        connect(socket, &QIODevice::readyRead,
                this, &LanLinkProvider::dataReceived);

        QTimer* timer = new QTimer(socket);
        timer->setSingleShot(true);
        timer->setInterval(1000);
        connect(socket, &QSslSocket::encrypted,
                timer, &QObject::deleteLater);
        connect(timer, &QTimer::timeout, socket, [socket] {
            qCWarning(KDECONNECT_CORE) << "LanLinkProvider/newConnection: Host timed out without sending any identity." << socket->peerAddress();
            socket->disconnectFromHost();
        });
        timer->start();
    }
}