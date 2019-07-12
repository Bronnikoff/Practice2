#pragma once
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <iomanip>
#include <vector>
#include <random>
#include <map>
#include <string>
using namespace std;

class RSA
{
public:
	RSA();
	~RSA();
	void Encrypt(vector<long>& messenge_encrypt, string& messenge); // Локальное шифрование на клиенте/сервере 
	void Decrypt(vector<long>& messenge_encrypt, string& messenge);  //	Локальная расшифровка на сервере/клиенте
	pair<long, long> MyOpenKey;   //  (e, n)
	pair<long, long> FriendOpenKey;   //  (e, n)

private:
	void GenerateKeys();
	int sundaram();
	long calculateE(long FEller);
	long GCD(long e, long FEller);
	long calculateD(long e, long FEller);
	long CharEncrypt(long i);
	long CharDecrypt(long i);

	pair<long, long> MyCloseKey;   //  (d, n)
};
