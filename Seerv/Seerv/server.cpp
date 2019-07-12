#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <ws2tcpip.h>
#include <winsock2.h>
#pragma comment(lib, "WSOCK32.lib")
#pragma comment(lib, "ws2_32.lib")
#include <sstream>
#include <string.h>
#include <windows.h>
#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <mysql.h>
#include <mutex>
#include <map>
#include "RSA.h"

#define PORT 53068
const unsigned int LIMIT = 5;

using namespace std;


typedef struct pack {
	std::mutex* mutex;
	MYSQL* mysql;
	SOCKET descriptor;
	map<unsigned, string>* connected;
	unsigned id;
	RSA* rsa;
}Package;


bool MakeReg(Package* item) {
	cout << "Choise #1" << endl;
	// получаем размер логина
	int size;
	if (recv(item->descriptor, (char*)& size, sizeof(int), 0) != sizeof(int)) {
		perror((char*)"Size getting");
		return false;
	}
	// выделяем память под логин
	char* buffer = (char*)malloc(sizeof(char) * (size + 1));
	if (!buffer) {
		return false;
	}
	//получаем логин
	if (recv(item->descriptor, buffer, size * sizeof(char), 0) != size * sizeof(char)) {
		perror((char*)"Naming error");
		free(buffer);
		return false;
	}
	buffer[size] = '\0';
	//сохраняем его в стринг
	string login = buffer;
	free(buffer);
	//приимаем пароль
	size_t password = 0;
	if (recv(item->descriptor, (char*)& password, sizeof(size_t), 0) != sizeof(size_t)) {
		perror((char*)"Naming error");
		return false;
	}
	std::stringstream chid;
	chid << password;
	cout << "Connected login:" << login << endl;
	// Ищем в БД соответствующие логин
	string recv = "SELECT id FROM persons WHERE login = '";
	recv += login;
	recv += "'";
	//делаем запрос к БД
	item->mutex->lock();
	if (mysql_query(item->mysql, recv.c_str())) {
		cout << "Data Base request Error!" << endl;
		item->mutex->unlock();
		return false;
	}
	MYSQL_RES* res = mysql_store_result(item->mysql);
	if (!res) {
		cout << "Result store Error!" << endl;
		item->mutex->unlock();
		return false;
	}
	// если находим такой логин => ошибка регистрации
	if (mysql_num_rows(res) > 0) {
		mysql_free_result(res);
		item->mutex->unlock();
		cout << "person was in database" << endl;
		size = -1;
		if (send(item->descriptor, (char*)& size, sizeof(int), 0) != sizeof(int)) {
			perror((char*)"Writing error");
		}
		return false;
	}

	item->mutex->unlock();
	mysql_free_result(res);
	//помещаем в БД новую персону:
	recv = "INSERT INTO persons VALUES(NULL, '";
	recv += login;
	recv += "', ";
	recv += chid.str();
	recv += ")";
	item->mutex->lock();
	if (mysql_query(item->mysql, recv.c_str())) {
		cout << "Data Base INSERT Error!" << endl;
		item->mutex->unlock();
		return false;
	}
	// получаем АЙДИ нового клиента:
	recv = "SELECT id FROM persons WHERE login = '";
	recv += login;
	recv += "'";
	if (mysql_query(item->mysql, recv.c_str())) {
		cout << "Data Base request Error!" << endl;
		item->mutex->unlock();
		return false;
	}
	res = mysql_store_result(item->mysql);
	if (!res) {
		cout << "Data store result Base Error!" << endl;
		item->mutex->unlock();
		return false;
	}
	if (mysql_num_rows(res) <= 0) {
		mysql_free_result(res);
		item->mutex->unlock();
		return false;
	}
	MYSQL_ROW row = mysql_fetch_row(res);
	item->id = atoi(row[0]);
	item->mutex->unlock();
	cout << "ID: " << item->id << endl;
	mysql_free_result(res);
	// отправляем код успеха клиенту
	size = 1;
	if (send(item->descriptor, (char*)& size, sizeof(int), 0) != sizeof(int)) {
		perror((char*)"Writing error");
		return false;
	}
	//помещаем в множество подключенных пользователей
	item->mutex->lock();
	item->connected->insert(std::make_pair(item->id, login));
	item->mutex->unlock();
	return true;
}

bool MakeSign(Package* item) {
	//принимаем размер логина
	int size;
	cout << "Choise #2" << endl;
	if (recv(item->descriptor, (char*)& size, sizeof(int), 0) != sizeof(int)) {
		perror((char*)"Size getting");
		return false;
	}
	//принимаем логин
	char* buffer = (char*)malloc((int)sizeof(char) * (size + 1));
	if (!buffer) {
		return false;
	}

	if (recv(item->descriptor, buffer, size * sizeof(char), 0) != size * sizeof(char)) { // 
		perror((char*)"Naming error");
		free(buffer);
		return false;
	}
	buffer[size] = '\0';
	string login = buffer;
	free(buffer);
	// приниамаем пароль:
	/*buffer = (char*)malloc(sizeof(size_t));
	if (!buffer) {
		cout << "Alloc error!" << endl;
		return false;
	}*/
	size_t password = 0;
	if (recv(item->descriptor, (char*)& password, sizeof(size_t), 0) != sizeof(size_t)) {
		//free(buffer);
		perror((char*)"Passing error");
		return false;
	}
	//password = buffer;
	//free(buffer);
	// ищем людей с таким логином и паролем:
	std::stringstream chid;
	chid << password;
	cout << "Connected login: " << login << endl;
	string recv = "SELECT id FROM persons WHERE login = '";
	recv += login;
	recv += "' AND password = ";
	recv += chid.str();
	item->mutex->lock();
	if (mysql_query(item->mysql, recv.c_str())) {
		cout << "Data Base request Error!" << endl;
		item->mutex->unlock();
		return false;
	}
	MYSQL_RES* res = mysql_store_result(item->mysql);
	if (!res) {
		cout << "Data Base store Error!" << endl;
		item->mutex->unlock();
		size = -1;
		if (send(item->descriptor, (char*)& size, sizeof(int), 0) != sizeof(int)) {
			perror((char*)"Writing error");
		}
		return false;
	}
	// если таких нет, значит возвращаем пользователю ошибку:
	if (mysql_num_rows(res) == 0) {
		mysql_free_result(res);
		item->mutex->unlock();
		size = -1;
		if (send(item->descriptor, (char*)& size, sizeof(int), 0) != sizeof(int)) {
			perror((char*)"Writing error");
		}
		return false;
	}
	MYSQL_ROW row = mysql_fetch_row(res);
	item->id = atoi(row[0]);
	if (item->connected->find(item->id) != item->connected->end()) {
		item->mutex->unlock();
		size = -1;
		if (send(item->descriptor, (char*)& size, sizeof(int), 0) != sizeof(int)) {
			perror((char*)"Writing error");
		}
		cout << "User was connect yet!" << endl;
		return false;
	}
	cout << "ID: " << row[0] << endl;
	item->mutex->unlock();
	mysql_free_result(res);
	// если такие есть, то отправляем успех
	size = 1;
	if (send(item->descriptor, (char*)& size, sizeof(int), 0) != sizeof(int)) {
		perror((char*)"Writing error");
		return false;
	}
	item->mutex->lock();
	// и добавляем в список подключенных:
	item->connected->insert(std::make_pair(item->id, login));
	item->mutex->unlock();
	return true;
}

int GetCmd(Package* item) {
	int ch;
	if (recv(item->descriptor, (char*)(&ch), sizeof(int), 0) < 1) {
		std::cout << "player disconnected!" << std::endl;
		closesocket(item->descriptor);
		item->connected->erase(item->id);
		return 0;
	}
	return ch;
}

bool SendMessag(Package* item) {
	int ch = 4;
	cout << "Choise #4" << endl;
	//получаем размер логина отправителя:
	if (recv(item->descriptor, (char*)& ch, sizeof(int), 0) != sizeof(int)) {
		perror((char*)"Sending numb");
		return false;
	}
	// получаем логин получателя:
	char* buf = (char*)malloc(sizeof(char) * (ch + 1));
	if (recv(item->descriptor, (char*)buf, sizeof(char) * ch, 0) != sizeof(char) * ch) {
		perror((char*)"Sending");
		free(buf);
		return false;
	}
	buf[ch] = '\0';
	std::string buffer = buf;
	free(buf);
	// ищем id получателя в базе
	string recvest = "SELECT id FROM persons WHERE login = '";
	recvest += buffer;
	recvest += "'";
	item->mutex->lock();
	if (mysql_query(item->mysql, recvest.c_str())) {
		cout << "DB search error" << endl;
		item->mutex->unlock();
		return false;
	}
	MYSQL_RES* res = mysql_store_result(item->mysql);
	if (res == NULL) {
		cout << "DB result store error" << endl;
		item->mutex->unlock();
		return false;

	}
	item->mutex->unlock();
	// если нет таких айди, пишем об этом клиенту
	if (mysql_num_rows(res) == 0) {
		cout << "Search ID Error" << endl;
		return false;
	}
	// достаем id отправителя
	MYSQL_ROW row = mysql_fetch_row(res);
	std::stringstream chid;

	recvest = "UPDATE ";

	mysql_free_result(res);
	//chid << row[0];
	// str -> int
	//chid >> ch; 
	//chid.clear();
	chid << item->id;
	string perep = row[0];
	// составляем запрос на поиск непрочитанного сообщения
	recvest = "SELECT id, message FROM ";
	recvest += row[0];
	recvest += "_";
	recvest += chid.str();
	recvest += " WHERE status = true";
	item->mutex->lock();
	if (mysql_query(item->mysql, recvest.c_str())) {
		ch = -1;
		if (send(item->descriptor, (char*)& ch, sizeof(int), 0) != sizeof(int)) {
			perror((char*)"Sending numb");
			mysql_free_result(res);
			return false;
		}
		item->mutex->unlock();
		cout << "Base not created yet!" << endl;
		return true;
	}
	// достаем результат запроса:
	res = mysql_store_result(item->mysql);
	if (res == NULL) {
		item->mutex->unlock();
		perror((char*) "DB result error");
		return false;
	}
	// если нет сообщений, отправляем -1
	if (mysql_num_rows(res) == 0) {
		ch = -1;
		item->mutex->unlock();
		if (send(item->descriptor, (char*)& ch, sizeof(int), 0) != sizeof(int)) {
			perror((char*)"Sending numb");
			mysql_free_result(res);
			return false;
		}
		return true;
	}
	// шифруем сообщение: 
	row = mysql_fetch_row(res);
	item->mutex->unlock();
	buffer = row[1];
	vector<long> kod;
	item->rsa->Encrypt(kod, buffer);
	ch = kod.size();

	recvest = "DELETE FROM " + perep + "_" + chid.str() + " WHERE id = ";
	recvest += row[0];

	if (mysql_query(item->mysql, recvest.c_str())) {
		cout << "Reqvest error" << endl;
		return false;
	}

	// отправляем размер шифра
	if (send(item->descriptor, (char*)& ch, sizeof(int), 0) != sizeof(int)) {
		perror((char*)"Sending numb");
		mysql_free_result(res);
		return false;
	}
	// отправляем шифр:
	if (send(item->descriptor, (char*)kod.data(), sizeof(long) * ch, 0) != sizeof(long) * ch) {
		perror((char*)"Sending kod");
		mysql_free_result(res);
		return false;
	}
	mysql_free_result(res);
	return true;
}

bool GetMessag(Package* item) {
	int ch{};
	cout << "Choise #3" << endl;
	//получаем размер логина получателя
	if (recv(item->descriptor, (char*)& ch, sizeof(int), 0) != sizeof(int)) {
		perror((char*)"Sending numb");
		return false;
	}
	char* buf = (char*)malloc(sizeof(char) * (ch + 1));
	// получаем логин получателя
	if (recv(item->descriptor, (char*)buf, sizeof(char) * ch, 0) != sizeof(char) * ch) {
		perror((char*)"Sending");
		free(buf);
		return false;
	}
	buf[ch] = '\0';
	std::string buffer = buf;
	free(buf);
	// ищем по базе данных
	item->mutex->lock();
	string recvest = "SELECT id FROM persons WHERE login = '";
	recvest += buffer;
	recvest += "'";
	if (mysql_query(item->mysql, recvest.c_str())) {
		perror((char*)"DB error");
		return false;
	}
	MYSQL_RES* res = mysql_store_result(item->mysql);
	if (res == NULL) {
		perror((char*) "DB result error");
		return false;

	}
	//если в базе нет, возвращаем код ошибки
	if (mysql_num_rows(res) == 0) {
		cout << "Client not in DB" << endl;
		ch = -1;
		send(item->descriptor, (char*)& ch, sizeof(int), 0);
		cout << "Not int base" << endl;
		mysql_free_result(res);
		return true;
	}
	// получаем id получателя
	MYSQL_ROW row = mysql_fetch_row(res);
	item->mutex->unlock();
	std::stringstream chid;
	chid.clear();
	chid << item->id;
	// получаем размер шифрованного сообщения в байтах
	if (recv(item->descriptor, (char*)& ch, sizeof(int), 0) != sizeof(int)) {
		perror((char*)"Sending numb");
		return false;
	}
	// выделяем под него память
	std::vector<long> kod(ch);
	kod.resize(ch);
	//  получаем шифрованное сообщение
	if (recv(item->descriptor, (char*)kod.data(), sizeof(long) * ch, 0) != sizeof(long) * ch) {
		perror((char*)"Sending");
		return false;
	}
	std::string message;
	// декодируем сообщение:
	item->rsa->Decrypt(kod, message);
	// если переписка не существовала, создаем:
	///////////////////////////////////////////////////////////////////////////////////////////
	/*chid << item->id;
	recvest = "CREATE TABLE ";
	recvest += chid.str();
	recvest += "_";
	recvest += row[0];
	recvest += " ( message TEXT, status bool NOT NULL)";
	if (!mysql_query(item->mysql, recvest.c_str())) {
		cout << "Table created!" << endl;
	}*/
	////////////////////////////////////////////////////////////////////////////////////////////
	// добавляем в переписку сообщение:
	recvest = "INSERT INTO ";
	recvest += chid.str();
	recvest += "_";
	recvest += row[0];
	recvest += " VALUES (NULL, '";
	recvest += message.c_str();
	recvest += "', true)";
	item->mutex->lock();
	if (mysql_query(item->mysql, recvest.c_str())) {
		cout << "Database insert error!" << endl;
	}
	item->mutex->unlock();
	return true;
}

bool SendKey(Package* item) {
	cout << "Choise #7" << endl;
	// отправляем открытые ключи шифрования
	long message = item->rsa->MyOpenKey.first;
	if (send(item->descriptor, (const char*)& message, sizeof(long), 0) != sizeof(long)) {
		perror((char*)"Reading");
		return false;
	}
	message = item->rsa->MyOpenKey.second;
	if (send(item->descriptor, (const char*)& message, sizeof(long), 0) != sizeof(long)) {
		perror((char*)"Reading");
		return false;
	}
	return true;
}

bool GetKey(Package* item) {
	int ch;
	cout << "Choise #6" << endl;
	// принимае размер запрашиваемого логина
	if (recv(item->descriptor, (char*)& ch, sizeof(int), 0) != sizeof(int)) {
		perror((char*)"Naming");
		return false;
	}
	char* nik = (char*)malloc(sizeof(char) * (ch + 1));
	if (!nik) {
		perror((char*)"Alloc error");
		return false;
	}
	// определяем логин
	if (recv(item->descriptor, nik, sizeof(char) * ch, 0) != sizeof(char) * ch) {
		perror((char*)"Naming");
		free(nik);
		return false;
	}
	nik[ch] = '\0';
	std::string buffer = nik;
	free(nik);
	// ищем в бд айди того, к кому логинимся
	string recvest = "SELECT id FROM persons WHERE login = '";
	recvest += buffer;
	recvest += "'";
	item->mutex->lock();
	if (mysql_query(item->mysql, recvest.c_str())) {
		cout << "DB search error" << endl;
		item->mutex->unlock();
		return true;
	}
	MYSQL_RES* res = mysql_store_result(item->mysql);
	if (res == NULL) {
		cout << "DB result store error" << endl;
		item->mutex->unlock();
		return false;

	}
	if (mysql_num_rows(res) == 0) {
		ch = -1;
		item->mutex->unlock();
		if (send(item->descriptor, (char*)& ch, sizeof(int), 0) != sizeof(int)) {
			perror((char*)"Sending numb");
			mysql_free_result(res);
			return false;
		}
		return true;
	}
	// достаем id
	MYSQL_ROW row = mysql_fetch_row(res);
	item->mutex->unlock();
	stringstream chid;
	chid << item->id;
	recvest = "CREATE TABLE IF NOT EXISTS ";
	recvest += chid.str();
	recvest += "_";
	recvest += row[0];
	recvest += " ( id INT PRIMARY KEY AUTO_INCREMENT, message TEXT, status bool NOT NULL)";
	if (!mysql_query(item->mysql, recvest.c_str())) {
		cout << "Table created!" << endl;
	}
	else {
		cout << "Table create error" << endl;
	}

	mysql_free_result(res);


	ch = 1;
	if (send(item->descriptor, (char*)& ch, sizeof(int), 0) != sizeof(int)) {
		perror((char*)"Sending numb");
		return false;
	}

	if (recv(item->descriptor, (char*)& item->rsa->FriendOpenKey.first, sizeof(long), 0) != sizeof(long)) {
		perror((char*)"Sending key");
		return false;

	}if (recv(item->descriptor, (char*)& item->rsa->FriendOpenKey.second, sizeof(long), 0) != sizeof(long)) {
		perror((char*)"Sending key");
		return false;
	}
	//cout << "My:" << endl;
	//cout << item->rsa->MyOpenKey.first << endl;
	//cout << item->rsa->MyOpenKey.second << endl;

	//cout << "Friend:" << endl;
	//cout << item->rsa->FriendOpenKey.first << endl;
	//cout << item->rsa->FriendOpenKey.second << endl;

	return true;
}

DWORD WINAPI Thread(LPVOID package) {
	Package* item = (Package*)package;
	item->rsa = new RSA;
	int ch;
	if (recv(item->descriptor, (char*)& ch, sizeof(int), 0) != sizeof(int)) { // принимаем comand
		perror((char*)"Size getting");
		closesocket(item->descriptor);
		delete item->rsa;
		free(item);
		return 0;
	}
	if (ch == 1) {
		if (!MakeReg(item)) {
			delete item->rsa;
			free(item);
			closesocket(item->descriptor);
			return 0;
		}
	}
	else if (ch == 2) {
		if (!MakeSign(item)) {
			delete item->rsa;
			free(item);
			closesocket(item->descriptor);
			return 0;
		}
	}
	else {
		free(item);
		closesocket(item->descriptor);
		return 0;
	}
	while (1) {
		if (!(ch = GetCmd(item))) {
			free(item);
			return 0;
		}
		if (ch == 3) {
			if (!GetMessag(item)) {
				break;
			}
			continue;
		}
		else if (ch == 4) {
			if (!SendMessag(item)) {
				break;
			}
			continue;
		}
		else if (ch == 5) {
			printf("Choise #5\n");
			break;
		}
		else if (ch == 6) {
			if (!GetKey(item)) {
				break;
			}
			continue;
		}
		else if (ch == 7) {
			if (!SendKey(item)) {
				break;
			}
			continue;
		}
	}
	closesocket(item->descriptor);
	item->connected->erase(item->id);
	delete item->rsa;
	free(item);
	return 0;
}


SOCKET ListenSpace() {
	struct sockaddr_in addr; //структура для инициализации сокета в bind
	SOCKET socketDescriptor; // дескриптор сокета
	socketDescriptor = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // создаем(открываем) сокет
	if (socketDescriptor == -1) {
		perror((char*)"Socket error");
		return false;
	}
	memset(&addr, 0, sizeof(struct sockaddr_in)); //обнуляем структуру(хз зачем)
	addr.sin_family = AF_INET; //указываем, что сокет подключеаем для внутренней работы в системе(не для интернета)
	addr.sin_port = htons(PORT);
	addr.sin_addr.s_addr = 0;

	if (bind(socketDescriptor, (struct sockaddr*) & addr, sizeof(addr)) == -1) { //подключаем сокет в ОС
		perror((char*)"Binding error");
		closesocket(socketDescriptor);
		return -1;
	}

	if (listen(socketDescriptor, LIMIT) == -1) { //говорим, что сокет готов принимать запросы(LIMIT запросов одновременно)
		perror((char*)"Listen error");
		closesocket(socketDescriptor);
		return -1;
	}

	//выводим принимающий порт:
	int mod = sizeof(sockaddr_in);
	memset(&addr, 0, sizeof(struct sockaddr_in));
	if (getsockname(socketDescriptor, (struct sockaddr*) & addr, &mod) == -1)
	{
		printf("getsockname error: %d\n", errno);
		closesocket(socketDescriptor);
		return -1;
	}
	std::cout << "Server PORT: " << htons(addr.sin_port) << std::endl;
	return socketDescriptor;
}


bool WriteIP() {
	char HostName[1024]; // создаем буфер для имени хоста    
	if (!gethostname(HostName, 1024)) // получаем имя хоста  
	{
		if (LPHOSTENT lphost = gethostbyname(HostName)) { // получаем IP хоста, т.е. нашего компа
			std::cout << "Server IP:" << inet_ntoa(*((in_addr*)lphost->h_addr_list[0])) << std::endl; // преобразуем переменную типа LPIN_ADDR в DWORD   
			return true;
		}
	}
	return false;
}


void DestructProg(MYSQL* mysql) {
	mysql_close(mysql);
	WSACleanup();
	std::cout << "Exit!" << std::endl;
	exit(1);
}

int main() {
	WSADATA	WsaData;
	WSAStartup(MAKEWORD(2, 2), &WsaData);
	std::mutex mutex;
	MYSQL* mysql;
	const char pass[] = "tnh123";
	const char Host[] = "localhost";
	const char DBase[] = "users";
	const char User[] = "root";
	std::map<unsigned, string> connected;
	mysql = mysql_init(NULL);
	if (!mysql_real_connect(mysql, Host, User, pass, DBase, 0, NULL, 0)) {
		cout << "Database connection error!" << endl;
		return 0;
	}
	// получаем IP server:
	if (!WriteIP()) {
		DestructProg(mysql);
	}
	// Listen port and get descriptor:
	SOCKET socketDescriptor = ListenSpace();
	if (socketDescriptor == -1) {
		DestructProg(mysql);
	}
	while (true) {
		SOCKET sockdesk; //принимаем коннект
		do {
			sockdesk = accept(socketDescriptor, NULL, NULL);
		} while (sockdesk < 0);
		Package* pack = (Package*)malloc(sizeof(Package));
		pack->descriptor = sockdesk;
		pack->mutex = &mutex;
		pack->mysql = mysql;
		pack->connected = &connected;
		DWORD ID;
		CreateThread(NULL, NULL, Thread, pack, NULL, &ID);
	}
	DestructProg(mysql);
}
