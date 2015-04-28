#ifndef SERVER_H
#define SERVER_H

//set lean and mean so that we can include window.h without defaulting to winsock version 1
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <iostream>
#include <string>
#include <sstream>
#include <chrono>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#include "sqlite3.h"

//required for ws2tcpip.. i think?
#pragma comment(lib, "Ws2_32.lib")

//constant definitions
#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 512

class Server
{
public:
	Server();

	bool Start();

	void Listen();

	void Receive();

	bool Stop();

	std::string getStatus();

	int getNumConnections();

	static int callback(void *NotUsed, int argc, char **argv, char **azColName);

	~Server();

private:
	//our database variables
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;

	//variables for initializing winsock
	WSADATA wsaData;
	int iResult;

	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;

	struct addrinfo *result = NULL;
	struct addrinfo hints;

	int iSendResult;
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;

	//variables for server management
	std::string serverStatus = "NOT STARTED";
	int numOfConnections = 0;
};

#endif