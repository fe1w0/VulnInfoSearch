void CClient::EchoMessage(const CMessage& Message) {
    CMessage EchoedMessage = Message;
    for (CClient* pClient : GetClients()) {
        if (pClient->HasEchoMessage() ||
            (pClient != this && (m_pNetwork->IsChan(Message.GetParam(0)) ||
                                 pClient->HasSelfMessage()))) {
            EchoedMessage.SetNick(GetNickMask());
            pClient->PutClient(EchoedMessage);
        }
    }
}