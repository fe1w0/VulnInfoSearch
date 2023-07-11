void ModuleSQL::init()
{
	if (mysql_library_init(0, NULL, NULL))
		throw ModuleException("Unable to initialise the MySQL library!");

	Dispatcher = new DispatcherThread(this);
	ServerInstance->Threads->Start(Dispatcher);

	Implementation eventlist[] = { I_OnRehash, I_OnUnloadModule };
	ServerInstance->Modules->Attach(eventlist, this, sizeof(eventlist)/sizeof(Implementation));

	OnRehash(NULL);
}