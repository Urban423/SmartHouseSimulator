#pragma once
#include "Client.h"
#include "Server.h"

enum AppMode
{
    ClientOnly,
    ServerOnly,
    Host
};

class App {
public:
	void onCreate();
	void onUpdate();
private:
	Client client;
	Server server;
};