bool CClient::OnTextMessage(CTextMessage& Message) {
    CString sTargets = Message.GetTarget();

    VCString vTargets;
    sTargets.Split(",", vTargets, false);

    for (CString& sTarget : vTargets) {
        Message.SetTarget(sTarget);
        if (m_pNetwork) {
            // May be nullptr.
            Message.SetChan(m_pNetwork->FindChan(sTarget));
        }

        if (sTarget.TrimPrefix(m_pUser->GetStatusPrefix())) {
            if (sTarget.Equals("status")) {
                CString sMsg = Message.GetText();
                UserCommand(sMsg);
            } else {
                CALLMOD(sTarget, this, m_pUser, m_pNetwork,
                        OnModCommand(Message.GetText()));
            }
            continue;
        }

        bool bContinue = false;
        NETWORKMODULECALL(OnUserTextMessage(Message), m_pUser, m_pNetwork, this,
                          &bContinue);
        if (bContinue) continue;

        if (!GetIRCSock()) {
            // Some lagmeters do a PRIVMSG to their own nick, ignore those.
            if (!sTarget.Equals(m_sNick))
                PutStatus(
                    t_f("Your message to {1} got lost, you are not connected "
                        "to IRC!")(Message.GetTarget()));
            continue;
        }

        if (m_pNetwork) {
            AddBuffer(Message);
            EchoMessage(Message);
            PutIRC(Message.ToString(CMessage::ExcludePrefix |
                                    CMessage::ExcludeTags));
        }
    }

    return true;
}