TEST_F(ZNCTest, StatusEchoMessage) {
    auto znc = Run();
    auto ircd = ConnectIRCd();
    auto client = LoginClient();
    client.Write("CAP REQ :echo-message");
    client.Write("PRIVMSG *status :blah");
    client.ReadUntil(":nick!user@irc.znc.in PRIVMSG *status :blah");
    client.ReadUntil(":*status!znc@znc.in PRIVMSG nick :Unknown command");
}