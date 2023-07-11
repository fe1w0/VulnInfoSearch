  void DefaultEnv::Initialize()
  {
    sLog           = new Log();
    SetUpLog();

    sEnv           = new DefaultEnv();
    sForkHandler   = new ForkHandler();
    sFileTimer     = new FileTimer();
    sPlugInManager = new PlugInManager();

    sPlugInManager->ProcessEnvironmentSettings();
    sForkHandler->RegisterFileTimer( sFileTimer );

    //--------------------------------------------------------------------------
    // MacOSX library loading is completely moronic. We cannot dlopen a library
    // from a thread other than a main thread, so we-pre dlopen all the
    // libraries that we may potentially want.
    //--------------------------------------------------------------------------
#ifdef __APPLE__
    char *errBuff = new char[1024];

    const char *libs[] =
    {
      "libXrdSeckrb5.so",
      "libXrdSecgsi.so",
      "libXrdSecgsiAuthzVO.so",
      "libXrdSecgsiGMAPDN.so",
      "libXrdSecgsiGMAPLDAP.so",
      "libXrdSecpwd.so",
      "libXrdSecsss.so",
      "libXrdSecunix.so",
      0
    };

    for( int i = 0; libs[i]; ++i )
    {
      sLog->Debug( UtilityMsg, "Attempting to pre-load: %s", libs[i] );
      bool ok = XrdOucPreload( libs[i], errBuff, 1024 );
      if( !ok )
        sLog->Error( UtilityMsg, "Unable to pre-load %s: %s", libs[i], errBuff );
    }
    delete [] errBuff;
#endif
  }