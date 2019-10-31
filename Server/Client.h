#pragma once
#include <WinSock2.h>
#include <Windows.h>
#include <memory>
#include <list>
#include "Thread.h"
#include <string>

class Client
{
public:
	explicit Client(SOCKET socket, std::string name);
	~Client();
	DWORD start(LPVOID mParams);
	void setThreadHandle(Thread<Client> *ptr);
	std::string name() const;

private:
	bool validResult(const int result);

private:
	std::string		_name;
	SOCKET			cSocket;
	Thread<Client>	*threadPtr;
};

typedef std::shared_ptr<Client> SClientPtr;
typedef std::list<SClientPtr> ClientsList;