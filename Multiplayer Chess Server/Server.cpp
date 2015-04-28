#include "Server.h"


Server::Server()
{
	//set up the database
	rc = sqlite3_open("userData.db", &db);
	if (rc)
	{
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
	}

	//set up things for winsock2
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;
}

bool Server::Start()
{
	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) { //Error Check!
		printf("WSAStartup failed: %d\n", iResult);
	}
	else
	{
		// Resolve the local address and port to be used by the server
		iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result); //Error Check!
		if (iResult != 0) {
			printf("getaddrinfo failed: %d\n", iResult);
			WSACleanup();
			return false;
		}
		else
		{
			// Create a SOCKET for the server to listen for client connections
			ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
			if (ListenSocket == INVALID_SOCKET) { //Error Check!
				printf("Error at socket(): %ld\n", WSAGetLastError());
				freeaddrinfo(result);
				WSACleanup();
				return false;
			}
			else
			{
				// Setup the TCP listening socket
				iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
				if (iResult == SOCKET_ERROR) { //Error Check!
					printf("bind failed with error: %d\n", WSAGetLastError());
					freeaddrinfo(result);
					closesocket(ListenSocket);
					WSACleanup();
					return false;
				}
				else
				{
					//free memory that was claimed by getaddrinfo()
					freeaddrinfo(result);
				}
			}
		}
	}

	//update server status
	serverStatus = "Server Initialized; Awaiting connection on port 27015.";

	return true;
}

void Server::Listen()
{
	//Listen on the socket and error check at the same time
	if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
		printf("Listen failed with error: %ld\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
	}
	else
	{
		// Accept a client socket -- We'll add these to a vector
		ClientSocket = accept(ListenSocket, NULL, NULL);
		if (ClientSocket == INVALID_SOCKET) {
			printf("accept failed: %d\n", WSAGetLastError());
			closesocket(ListenSocket);
			WSACleanup();
		}
		else ++numOfConnections;
	}
}

void Server::Receive()
{
	// Receive until the peer shuts down the connection
	do {
		
		//iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
		iResult = recv(ClientSocket, recvbuf, sizeof(recvbuf), 0);
			

		if (iResult > 0) {

			//recvbuf[iResult] = '\0';

			printf("Bytes received: %d\nMessage received: %s\n", iResult, recvbuf);

			//This is where we parse the message
			if (recvbuf[0] == '#')
			{
				printf("A server command was received!");

				std::string str;

				str = recvbuf;

				//sstr.str("");

				//sstr << recvbuf;

				bool eof = false;
				int i = 1;

				while (!eof)
				{
					if (recvbuf[i] == '#')
					{
						//check the accumulated string to see what the command is
						printf("%s", str);

						//reset string
						str = "";
					}
					else if (recvbuf[i] == '*')
					{
						//end of line character reached!
						eof = true;
					}
					else
					{
						//push it back into a string 
						str += recvbuf[i];
					}

					//increment our counter
					++i;
				}

				rc = sqlite3_exec(db, "SQL GOES HERE", callback, 0, &zErrMsg);
				
				if (rc != SQLITE_OK)
				{
					fprintf(stderr, "SQL error: %s\n", zErrMsg);
					sqlite3_free(zErrMsg);
				}

				printf("Converted string: %s", str);
			}
			else
			{
				// Echo the buffer back to the sender, this will eventually
				// be used to echo chat between clients.
				iSendResult = send(ClientSocket, recvbuf, iResult, 0);
				if (iSendResult == SOCKET_ERROR) {
					printf("send failed: %d\n", WSAGetLastError());
					closesocket(ClientSocket);
					WSACleanup();
				}
				printf("Bytes sent: %d\n", iSendResult);
			}
		}
		else if (iResult == 0)
			printf("Connection closing...\n");
		else {
			printf("recv failed: %d\n", WSAGetLastError());
			closesocket(ClientSocket);
			WSACleanup();
		}

	} while (iResult > 0);
}

bool Server::Stop()
{
	// shutdown the send half of the connection since no more data will be sent
	iResult = shutdown(ClientSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed: %d\n", WSAGetLastError());
		closesocket(ClientSocket);
		WSACleanup();
		return false;
	}

	// cleanup
	closesocket(ClientSocket);
	WSACleanup();

	//change server status
	serverStatus = "NOT STARTED";

	return true;
}

std::string Server::getStatus()
{
	return serverStatus;
}

int Server::getNumConnections()
{
	return numOfConnections;
}

int Server::callback(void *NotUsed, int argc, char **argv, char **azColName)
{
	//this is where the code to handle the tables created by SQLite goes
	int i;
	for (i = 0; i<argc; i++)
	{
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
	printf("\n");
	return 0;
}

Server::~Server()
{
	//close connection to database
	sqlite3_close(db);
}
