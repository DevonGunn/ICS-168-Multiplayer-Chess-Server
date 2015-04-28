/**
##############################################

C++ Server for Multiplayer Chess

Written by Devon Gunn (Student ID#28502436)

##############################################
**/

//set lean and mean so that we can include window.h without defaulting to winsock version 1
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <iostream>
#include <string>
#include <chrono>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#include "Server.h"

//required for ws2tcpip.. i think?
#pragma comment(lib, "Ws2_32.lib")

//constant definitions
#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 512

//declaration for a function to redraw the menu
//we pass in the current server status
void redrawMenu(std::string status, int numConnections);

int main(int argc, char *argv[])
{
	//variables
	Server *server = new Server();

	bool updateServerDisplay = true; //starts true for initial draw
	bool started = false;
	time_t onePressTimer = clock();

	

	

	//server loops until we press insert
	while (!GetAsyncKeyState(VK_INSERT))
	{
		if (updateServerDisplay)
		{
			system("cls");
			std::cout << "----------------------------------------------------" << std::endl;
			std::cout << "           Multiplayer Chess Server!" << std::endl;
			std::cout << "----------------------------------------------------" << std::endl << std::endl;
			std::cout << "SERVER STATUS: [" << server->getStatus() << "]" << std::endl << std::endl;
			std::cout << "# of Connections: [" << server->getNumConnections() << "]" << std::endl << std::endl;
			std::cout << "[F1] Start Server" << std::endl << std::endl;
			std::cout << "[F2] Stop Server" << std::endl << std::endl;
			std::cout << "[INSERT] Exit" << std::endl << std::endl;
			std::cout << "----------------------------------------------------" << std::endl;
			std::cout << "             Written by Devon Gunn" << std::endl;
			std::cout << "----------------------------------------------------" << std::endl;

			//toggle update flag off
			updateServerDisplay = false;
		}

		//Use timer to prevent button from being pressed multiple times in one press
		if (clock() - onePressTimer > 400)
		{
			//Handle key presses for server commands
			if (GetAsyncKeyState(VK_F1) && !started) //SERVER START
			{
				//this is where we'll start the server
				if (server->Start())
				{
					started = true;
					server->Listen();
					if (server->getNumConnections() > 0) server->Receive();
				}
				else printf("Error: Unable to start Server!\n");

				//tell server to redraw display with updated text
				updateServerDisplay = true;

				//reset press timer
				onePressTimer = clock();
			}
			else if (GetAsyncKeyState(VK_F2) && started) //SERVER STOP
			{
				//this is where we'll stop the server
				if (server->Stop()) started = false;
				else printf("Error: Unable to stop Server!\n");

				//tell server to redraw display with updated text
				updateServerDisplay = true;

				//reset press timer
				onePressTimer = clock();
			}
		}
	}

	//clean up
	delete server;
	server = NULL;

}