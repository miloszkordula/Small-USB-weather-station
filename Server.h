#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment (lib, "ws2_32.lib")
#include <WinSock2.h>
#include "FileManager.h"

class Server
{
	WSADATA wsaData;
    sockaddr_in serverAddress;
	SOCKET mainSocket;
	Sensor sensor;
	FileManager fileManager;
	int newHistory = 0;
	int updateSuccessful = 0;

public:
	int createServer();
	int handleClient();
};

