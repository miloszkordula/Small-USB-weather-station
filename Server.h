#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment (lib, "ws2_32.lib")
#include <WinSock2.h>
#include "Sensor.h"
class Server
{
	WSADATA wsaData;
    sockaddr_in serverAddress;
	SOCKET mainSocket;
	Sensor sensor;
public:
	int createServer();
	int handleClient(const char* comPort);
	int saveReadingsToFile(char readingsJSON[256]);

};

