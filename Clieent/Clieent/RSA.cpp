#include "RSA.h"


RSA::RSA()
{
	GenerateKeys();
}

RSA::~RSA() {}

void RSA::GenerateKeys()
{
	pair<long, long> OpenKey;    //  (e, n)

	long p = sundaram();
	long q = sundaram();
	long n = p * q;
	long FEller = (p - 1) * (q - 1);

	long e = this->calculateE(FEller);
	long d = this->calculateD(e, FEller);

	OpenKey.first = e;
	OpenKey.second = n;

	this->MyCloseKey.first = d;
	this->MyCloseKey.second = n;

	this->MyOpenKey.first = e;
	this->MyOpenKey.second = n;
}


void RSA::Encrypt(vector<long> & messenge_encrypt, string & messenge)
{
	for (size_t i = 0; i < messenge.size(); ++i) {
		messenge_encrypt.push_back(CharEncrypt(static_cast<long>(messenge[i])));
	}
}

void RSA::Decrypt(vector<long> & messenge_encrypt, string & messenge)
{
	for (size_t i = 0; i < messenge_encrypt.size(); ++i) {
		messenge += static_cast<char>(CharDecrypt(messenge_encrypt[i]));
	}
}




//Алгоритм "решето Сундарама". Выбирает все простые числа
//до заданного (случайно сгенерированного).
int RSA::sundaram()
{
	random_device random_device;
	mt19937 generator(random_device());
	uniform_int_distribution<> distribution(0, 100);
	int n = distribution(generator);
	int* a = new int[n];
	int k;
	memset(a, 0, sizeof(int) * n);

	for (int i = 1; 3 * i + 1 < n; i++)
	{
		for (int j = 1; (k = i + j + 2 * i * j) < n && j <= i; j++)
			a[k] = 1;
	}

	//Выбирает из списка простых чисел ближайшее к заданному.
	for (int i = n - 1; i >= 1; i--) {
		if (a[i] == 0) {
			delete[] a;
			return (2 * i + 1);
		}
	}
}


// Выбирается целое число e ( 1 < e < FEller ) // взаимно простое со значением функции Эйлера (FEller)
long RSA::calculateE(long FEller)
{
	long e;

	for (e = 2; e < FEller; ++e)
	{
		if (GCD(e, FEller) == 1)
		{
			return e;
		}
	}

	return -1;
}

long RSA::GCD(long e, long FEller)
{
	long myTemp;
	while (e > 0) {
		myTemp = e;
		e = FEller % e;
		FEller = myTemp;
	}

	return FEller;
}


// Вычисляется число d, мультипликативно обратное к числу e по модулю φ(n), то есть число, удовлетворяющее сравнению:
//    d ⋅ e ≡ 1 (mod φ(n))
long RSA::calculateD(long e, long FEller)
{
	long d;
	long k = 1;
	while (true) {
		k += FEller;
		if (k % e == 0) {
			d = (k / e);
			return d;
		}
	}
	//  e * (d = (k = n*FEller + 1) / e) mod FEller = 1
}


long RSA::CharEncrypt(long messenge)  //  e  n
{
	long result = 1;

	for (long j = 0; j < this->FriendOpenKey.first; j++) {
		result *= messenge;
		result %= this->FriendOpenKey.second;
	}


	return result;
}

long RSA::CharDecrypt(long CharEncrypt_messenge)
{
	long result = 1;

	for (long j = 0; j < this->MyCloseKey.first; j++) {
		result *= CharEncrypt_messenge;
		result %= this->MyCloseKey.second;
	}

	return result;
}
