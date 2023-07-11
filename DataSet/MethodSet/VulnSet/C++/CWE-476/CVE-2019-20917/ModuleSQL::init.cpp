void ModuleSQL::init()
{
	Dispatcher = new DispatcherThread(this);
	ServerInstance->Threads->Start(Dispatcher);

	Implementation eventlist[] = { I_OnRehash, I_OnUnloadModule };
	ServerInstance->Modules->Attach(eventlist, this, sizeof(eventlist)/sizeof(Implementation));

	OnRehash(NULL);
}