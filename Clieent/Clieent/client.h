#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS

//#include <ws2tcpip.h>
//#include <winsock2.h>
#pragma comment(lib, "WSOCK32.lib")
#pragma comment(lib, "ws2_32.lib")

#include <string.h>
#include <windows.h>
#include <iostream>
#include <string>
#include <vector>

//#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
//#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#include <ctype.h>



//#define PORT 53068
//#define SERVERADDR "192.168.0.17"

class ServerConnection {
public:
	ServerConnection(HWND hWinowMain);
	~ServerConnection();

	bool Server_Connect(std::string& nickname, size_t pass, bool registration);
	int GetDescriptor();
	void Server_Disconnect();

	bool Send_Message(std::vector<long>& MessIn, std::string& FriendLogin);
	bool Read_Message(std::vector<long>& MessIn, std::string& FriendLogin);
	bool Get_Open_Key(std::pair<long, long>& OpenKey, std::string& FriendLogin);
	bool Send_Open_Key(std::pair<long, long>& OpenKey, std::string& FriendLogin);

private:
	int socketDescriptor;
	const char servIP[16] = "172.18.5.225";
	bool busy;
	const unsigned port = 53068;
	sockaddr_in addr;
	HWND hWinowMain;
};
