#include "client.h"



ServerConnection::~ServerConnection() {
	if (busy) {
		int cmd = 5;
		send(socketDescriptor, (const char*)& cmd, sizeof(int), 0);
		closesocket(socketDescriptor);
	}
	WSACleanup();
}

ServerConnection::ServerConnection(HWND hWinowMain) {
	this->hWinowMain = hWinowMain;
	WSADATA	WsaData;
	WSAStartup(MAKEWORD(2, 2), &WsaData);

	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	HOSTENT* hst;

	
	if (inet_addr(servIP) != INADDR_NONE)
		addr.sin_addr.s_addr = inet_addr(servIP);
	else
	{
		
		if (hst = gethostbyname(servIP))
			// hst->h_addr_list �������� �� ������ �������,
			// � ������ ���������� �� ������
			((unsigned long*)& addr.sin_addr)[0] = ((unsigned long**)hst->h_addr_list)[0][0];
		else
		{
			MessageBox(this->hWinowMain, L"Invalid IP address", L"ERROR", MB_ICONINFORMATION | MB_OK);
			WSACleanup();
			return;
		}
	}
	busy = false;
}


int ServerConnection::GetDescriptor() {
	return socketDescriptor;
}



void ServerConnection::Server_Disconnect() {
	if (!busy) {
		MessageBox(hWinowMain, L"Cant disconnect! You diconnected now!", L"WARNING", MB_ICONINFORMATION | MB_OK);
		return;
	}
	closesocket(socketDescriptor);
	busy = false;
	MessageBox(hWinowMain, L"Access! You disconnected!", L"MESSAGE", MB_ICONINFORMATION | MB_OK);
}//done

bool ServerConnection::Server_Connect(std::string & nickname, size_t pass, bool registration) {
	using std::wstring;

	if (busy) {
		MessageBox(this->hWinowMain, L"Can't connect! Server connected now!", L"Information", MB_ICONINFORMATION | MB_OK);
		return true;
	}

	socketDescriptor = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (socketDescriptor == -1) {
		MessageBox(this->hWinowMain, L"Socket error", L"ERROR", MB_ICONINFORMATION | MB_OK);
		return false;
	}

	if (connect(socketDescriptor, (struct sockaddr*) & addr, sizeof(addr)) == -1) {
		MessageBox(this->hWinowMain, L"Error when connecting to the server", L"ERROR", MB_ICONINFORMATION | MB_OK);
		return false;
	}

	int cmd;
	if (registration)
		cmd = 1;
	else
		cmd = 2;
	// ���������� �������
	if (send(socketDescriptor, (char*)& cmd, sizeof(int), 0) != sizeof(int)) {
		MessageBox(this->hWinowMain, L"Sending error", L"ERROR", MB_ICONINFORMATION | MB_OK);
		closesocket(socketDescriptor);
		return false;
	}
	// ���������� ������ login
	int sz_l = nickname.size();
	if (send(socketDescriptor, (char*)& sz_l, sizeof(int), 0) != sizeof(int)) {
		MessageBox(this->hWinowMain, L"Sending error", L"ERROR", MB_ICONINFORMATION | MB_OK);
		closesocket(socketDescriptor);
		return false;
	}
	//���������� �����
	if (send(socketDescriptor, nickname.c_str(), sz_l * sizeof(char), 0) != sz_l * sizeof(char)) {
		MessageBox(this->hWinowMain, L"Sending error", L"ERROR", MB_ICONINFORMATION | MB_OK);
		closesocket(socketDescriptor);
		return false;
	}
	//���������� ��� �������
	if (send(socketDescriptor, (char*)& pass, sizeof(size_t), 0) != sizeof(size_t)) {
		MessageBox(this->hWinowMain, L"Sending error", L"ERROR", MB_ICONINFORMATION | MB_OK);
		closesocket(socketDescriptor);
		return false;
	}
	// ��������� ������
	if (recv(socketDescriptor, (char*)& cmd, sizeof(int), 0) != sizeof(int)) {
		MessageBox(this->hWinowMain, L"Info getting error", L"ERROR", MB_ICONINFORMATION | MB_OK);
		closesocket(socketDescriptor);
		return false;
	}
	// ���� �������
	if (cmd < 0) {
		closesocket(socketDescriptor);
		MessageBox(this->hWinowMain, L"Can't autorize user!", L"ERROR", MB_ICONINFORMATION | MB_OK);
		return false;
	}
	// ���� �����:
	MessageBox(this->hWinowMain, L"Server connect success!", L"SERVER ANWSER", MB_ICONINFORMATION | MB_OK);


	busy = true;
	return true;
}//done

bool ServerConnection::Send_Message(std::vector<long> & MessIn, std::string & FriendLogin) {
	if (FriendLogin.empty()) {
		MessageBox(this->hWinowMain, L"Imput friend's login", L"ERROR", MB_ICONINFORMATION | MB_OK);
		return false;
	}

	if (MessIn.empty()) {
		MessageBox(this->hWinowMain, L"Imput message", L"ERROR", MB_ICONINFORMATION | MB_OK);
		return false;
	}

	if (!busy) {
		MessageBox(this->hWinowMain, L"Server disconnected! Please, connect to server for sending messages!", L"ERROR", MB_ICONINFORMATION | MB_OK);
		return false;
	}

	// �������� � ������� �� ������
	int cmd = 3;
	if (send(socketDescriptor, (char*)(&cmd), sizeof(int), 0) != sizeof(int)) {
		MessageBox(this->hWinowMain, L"Choise write", L"ERROR", MB_ICONINFORMATION | MB_OK);
		closesocket(socketDescriptor);
		busy = false;
		return false;
	}


	//  �������� �� �� ��� ������������ ����� �������� ���������
	cmd = FriendLogin.size();
	if (send(socketDescriptor, (char*)(&cmd), sizeof(int), 0) != sizeof(int)) {
		MessageBox(this->hWinowMain, L"Write", L"ERROR", MB_ICONINFORMATION | MB_OK);
		closesocket(socketDescriptor);
		busy = false;
		return false;
	}
	if (send(socketDescriptor, FriendLogin.c_str(), sizeof(char) * cmd, 0) != sizeof(char) * cmd) {
		MessageBox(this->hWinowMain, L"Write", L"ERROR", MB_ICONINFORMATION | MB_OK);
		closesocket(socketDescriptor);
		busy = false;
		return false;
	}

	//recv(socketDescriptor, (char*) &cmd, sizeof(int), 0);
	//if (cmd < 0) {
	//	MessageBox(this->hWinowMain, L"It's not your friend!", L"ERROR", MB_ICONINFORMATION | MB_OK);
	//	return false;
	//}


	//  �������� ���������
	int size = MessIn.size();
	if (send(socketDescriptor, reinterpret_cast<char*>(&size), sizeof(int), 0) != sizeof(int)) {
		MessageBox(this->hWinowMain, L"Wrong recipient!", L"ERROR", MB_ICONINFORMATION | MB_OK);
		closesocket(socketDescriptor);
		busy = false;
		return false;
	}

	if (send(socketDescriptor, reinterpret_cast<char*>(MessIn.data()), sizeof(long) * size, 0) != sizeof(long) * size) {
		MessageBox(this->hWinowMain, L"Writing", L"ERROR", MB_ICONINFORMATION | MB_OK);
		closesocket(socketDescriptor);
		busy = false;
		return false;
	}
	MessageBox(this->hWinowMain, L"Message shipped!", L"Information", MB_ICONINFORMATION | MB_OK);
	return true;
}//done


bool ServerConnection::Read_Message(std::vector<long> & MessOut, std::string & FriendLogin) {
	if (FriendLogin.empty()) {
		MessageBox(this->hWinowMain, L"Imput friend's login", L"ERROR", MB_ICONINFORMATION | MB_OK);
		return false;
	}

	if (!MessOut.empty()) {
		MessOut.clear();
	}

	if (!busy) {
		MessageBox(this->hWinowMain, L"Server disconnected! Please, connect to server for reading messages!", L"ERROR", MB_ICONINFORMATION | MB_OK);
		return false;
	}


	// �������� � ������� �� ������
	int cmd = 4;
	if (send(socketDescriptor, (char*)(&cmd), sizeof(int), 0) != sizeof(int)) {
		MessageBox(this->hWinowMain, L"Choise write", L"ERROR", MB_ICONINFORMATION | MB_OK);
		closesocket(socketDescriptor);
		busy = false;
		return false;
	}


	//  �������� �� �� ��� �� ������������ ����� �������� ��������� (��� �������� ����)
	cmd = FriendLogin.size();
	if (send(socketDescriptor, (char*)& cmd, sizeof(int), 0) != sizeof(int)) {
		MessageBox(this->hWinowMain, L"Write", L"ERROR", MB_ICONINFORMATION | MB_OK);
		closesocket(socketDescriptor);
		busy = false;
		return false;
	}
	if (send(socketDescriptor, FriendLogin.c_str(), sizeof(char) * cmd, 0) != sizeof(char) * cmd) {
		MessageBox(this->hWinowMain, L"Write", L"ERROR", MB_ICONINFORMATION | MB_OK);
		closesocket(socketDescriptor);
		busy = false;
		return false;
	}
	// �������� -1 ��� ������ ��������� � ������
	if (recv(socketDescriptor, (char*)& cmd, sizeof(int), 0) == sizeof(int)) {
		if (cmd < 0) {
			//MessageBox(this->hWinowMain, L"It's not your friend!", L"ERROR", MB_ICONINFORMATION | MB_OK);
			return false;
		}
	}
	else {
		MessageBox(this->hWinowMain, L"Write", L"ERROR", MB_ICONINFORMATION | MB_OK);
		closesocket(socketDescriptor);
		busy = false;
		return false;
	}

	MessOut.resize(cmd);
	// ��������� �����:
	if (recv(socketDescriptor, (char*)(MessOut.data()), sizeof(long) * cmd, 0) < sizeof(long) * cmd) {
		MessageBox(this->hWinowMain, L"KOD Reding", L"ERROR", MB_ICONINFORMATION | MB_OK);
		closesocket(socketDescriptor);
		busy = false;
		return false;
	}

	return true;
}//done



bool ServerConnection::Send_Open_Key(std::pair<long, long> & OpenKey, std::string & FriendLogin)
{
	if (FriendLogin.empty()) {
		MessageBox(this->hWinowMain, L"Imput friend's login", L"ERROR", MB_ICONINFORMATION | MB_OK);
		return false;
	}

	if (!busy) {
		MessageBox(this->hWinowMain, L"Server disconnected! Please, connect to server for sending messages!", L"ERROR", MB_ICONINFORMATION | MB_OK);
		return false;
	}

	// �������� � ������� �� ������
	int cmd = 6;
	if (send(socketDescriptor, (char*)(&cmd), sizeof(int), 0) != sizeof(int)) {
		MessageBox(this->hWinowMain, L"Choise write", L"ERROR", MB_ICONINFORMATION | MB_OK);
		closesocket(socketDescriptor);
		busy = false;
		return false;
	}

	cmd = FriendLogin.size();
	if (send(socketDescriptor, (char*)(&cmd), sizeof(int), 0) != sizeof(int)) {
		MessageBox(this->hWinowMain, L"Size write", L"ERROR", MB_ICONINFORMATION | MB_OK);
		closesocket(socketDescriptor);
		busy = false;
		return false;
	}

	//  �������� �� �� ��� ������������ ����� �������� ��������� (��������������� �� ��, � ����� ������ ����� �� �� ������)
	if (send(socketDescriptor, FriendLogin.c_str(), sizeof(char) * FriendLogin.size(), 0) != sizeof(char) * FriendLogin.size()) {
		MessageBox(this->hWinowMain, L"Write", L"ERROR", MB_ICONINFORMATION | MB_OK);
		closesocket(socketDescriptor);
		busy = false;
		return false;
	}

	recv(socketDescriptor, (char*)& cmd, sizeof(int), 0);
	if (cmd < 0) {
		MessageBox(this->hWinowMain, L"It's not your friend!", L"ERROR", MB_ICONINFORMATION | MB_OK);
		return false;
	}


	//  �������� ���������
	long e = OpenKey.first;
	if (send(socketDescriptor, reinterpret_cast<char*>(&e), sizeof(long), 0) != sizeof(long)) {
		MessageBox(this->hWinowMain, L"Wrong recipient e!", L"ERROR", MB_ICONINFORMATION | MB_OK);
		closesocket(socketDescriptor);
		busy = false;
		return false;
	}

	long n = OpenKey.second;
	if (send(socketDescriptor, reinterpret_cast<char*>(&n), sizeof(long), 0) != sizeof(long)) {
		MessageBox(this->hWinowMain, L"Wrong recipient n!", L"ERROR", MB_ICONINFORMATION | MB_OK);
		closesocket(socketDescriptor);
		busy = false;
		return false;
	}

	MessageBox(this->hWinowMain, L"OpenKey shipped!", L"Information", MB_ICONINFORMATION | MB_OK);
	return true;
}//cmd

bool ServerConnection::Get_Open_Key(std::pair<long, long> & OpenKey, std::string & FriendLogin)
{
	if (FriendLogin.empty()) {
		MessageBox(this->hWinowMain, L"Imput friend's login", L"ERROR", MB_ICONINFORMATION | MB_OK);
		return false;
	}

	if (!busy) {
		MessageBox(this->hWinowMain, L"Server disconnected! Please, connect to server for reading messages!", L"ERROR", MB_ICONINFORMATION | MB_OK);
		return false;
	}


	// �������� � ������� �� ������
	int cmd = 7;
	if (send(socketDescriptor, (char*)(&cmd), sizeof(int), 0) != sizeof(int)) {
		MessageBox(this->hWinowMain, L"Choise write", L"ERROR", MB_ICONINFORMATION | MB_OK);
		closesocket(socketDescriptor);
		busy = false;
		return false;
	}

	//  ��������� ���������
	long e;
	recv(socketDescriptor, reinterpret_cast<char*>(&e), sizeof(long), 0);
	OpenKey.first = e;

	long n;
	recv(socketDescriptor, reinterpret_cast<char*>(&n), sizeof(long), 0);
	OpenKey.second = n;

	return true;
}//cmd