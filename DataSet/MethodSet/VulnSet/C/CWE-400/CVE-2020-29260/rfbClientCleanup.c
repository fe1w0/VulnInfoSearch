void rfbClientCleanup(rfbClient* client) {
#ifdef LIBVNCSERVER_HAVE_LIBZ
  int i;

  for ( i = 0; i < 4; i++ ) {
    if (client->zlibStreamActive[i] == TRUE ) {
      if (inflateEnd (&client->zlibStream[i]) != Z_OK &&
	  client->zlibStream[i].msg != NULL)
	rfbClientLog("inflateEnd: %s\n", client->zlibStream[i].msg);
    }
  }

  if ( client->decompStreamInited == TRUE ) {
    if (inflateEnd (&client->decompStream) != Z_OK &&
	client->decompStream.msg != NULL)
      rfbClientLog("inflateEnd: %s\n", client->decompStream.msg );
  }
#endif

  if (client->ultra_buffer)
    free(client->ultra_buffer);

  if (client->raw_buffer)
    free(client->raw_buffer);

  FreeTLS(client);

  while (client->clientData) {
    rfbClientData* next = client->clientData->next;
    free(client->clientData);
    client->clientData = next;
  }

  if (client->sock != RFB_INVALID_SOCKET)
    rfbCloseSocket(client->sock);
  if (client->listenSock != RFB_INVALID_SOCKET)
    rfbCloseSocket(client->listenSock);
  free(client->desktopName);
  free(client->serverHost);
  if (client->destHost)
    free(client->destHost);
  if (client->clientAuthSchemes)
    free(client->clientAuthSchemes);

#ifdef LIBVNCSERVER_HAVE_SASL
  if (client->saslSecret)
    free(client->saslSecret);
#endif /* LIBVNCSERVER_HAVE_SASL */

#ifdef WIN32
  if(WSACleanup() != 0) {
      errno=WSAGetLastError();
      rfbClientErr("Could not terminate Windows Sockets: %s\n", strerror(errno));
  }
#endif

  free(client);
}