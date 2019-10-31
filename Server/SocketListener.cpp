#include "SocketListener.h"
#include <WS2tcpip.h>
#include "Thread.h"
#include "Client.h"
#include "MessageQueue.h"
#include "namespaces.h"

SocketListener::SocketListener() : _socketBinded(false)
{
	this->_initEvent = CreateEvent(NULL, FALSE, FALSE, "AppSocketListenerInitEvent");
	this->_createSocketEvent = CreateEvent(NULL, FALSE, FALSE, "AppSocketListenerCreateSocketEvent");
	this->_socketBindEvent = CreateEvent(NULL, FALSE, FALSE, "AppSocketListenerSocketBindEvent");
	WSADATA data;
	WORD ver = MAKEWORD(2, 2);
	int initResult = WSAStartup(ver, &data);
	if (initResult != 0)
	{
		Messages::Message msg{ Messages::ERRROR, GetLastError(), AppErrors::WSA_INIT_ERROR };
		MSGQUEUE->postMessage(msg);
	}

	SetEvent(this->_initEvent);

	sListen = socket(AF_INET, SOCK_STREAM, 0);
	if (sListen == INVALID_SOCKET)
	{
		Messages::Message msg{ Messages::ERRROR, GetLastError(), AppErrors::SOCKET_INIT_ERROR };
		MSGQUEUE->postMessage(msg);
	}
	SetEvent(this->_createSocketEvent);
}

SocketListener::~SocketListener()
{
	closesocket(this->sListen);
	WSACleanup();
}

DWORD SocketListener::start(LPVOID mParams)
{
	Params* params = (Params*)mParams;
	aListen.sin_family = AF_INET;
	aListen.sin_port = htons(params->port);
	aListen.sin_addr.S_un.S_addr = INADDR_ANY;
	
	int bindResult = bind(
		this->sListen,
		(SOCKADDR*)&this->aListen,
		sizeof(this->aListen)
	);
	if (bindResult == -1)
	{
		Messages::Message msg{ Messages::ERRROR, GetLastError(), AppErrors::SOCKET_BIND_ERROR };
		MSGQUEUE->postMessage(msg);
		_socketBinded = false;
		return -1;
	}

	_socketBinded = true;
	SetEvent(this->_socketBindEvent);

	while (!MSGQUEUE->isExitFlag())
	{
		listen(this->sListen, SOMAXCONN);
		this->acceptNewConnection();
	}

	return 0;
}

HANDLE SocketListener::initEvent() const
{
	return this->_initEvent;
}

HANDLE SocketListener::createSocketEvent() const
{
	return this->_createSocketEvent;
}

HANDLE SocketListener::socketBindEvent() const
{
	return this->_socketBindEvent;
}

bool SocketListener::isSocketBinded() const
{
	return _socketBinded;
}

bool SocketListener::isSocketValid() const
{
	return this->sListen != INVALID_SOCKET;
}

void SocketListener::acceptNewConnection()
{
	SOCKADDR_IN clientAddr;
	int clientAddrSize = sizeof(clientAddr);
	SOCKET clientSocket = accept(this->sListen, (SOCKADDR*)&clientAddr, &clientAddrSize);
	if (clientSocket == INVALID_SOCKET)
	{
		Messages::Message msg{ Messages::ERRROR, GetLastError(), AppErrors::CONNECTION_CLOSED };
		MSGQUEUE->postMessage(msg);
		return;
	}

	std::string host = getClientHost(clientAddr);
	std::string port = getClientPort(clientAddr);

	Messages::Message msg{ Messages::NOTYFICATION, 0, AppMessages::CONNECTED + TEXT(": ") + host + TEXT(" ") + AppMessages::OPENED_PORT + TEXT(": ") + port };
	MSGQUEUE->postMessage(msg);
	
	SClientPtr client = std::make_shared<Client>(clientSocket, host);
	Thread<Client> *thread = new Thread<Client>(client.get(), &Client::start, NULL);
	if (!thread->start())
	{
		Messages::Message msg{ Messages::ERRROR, GetLastError(), AppErrors::THREAD_RUNNING_ERROR };
		MSGQUEUE->postMessage(msg);
	}
	client->setThreadHandle(thread);
	_clients.push_back(client);
}

DWORD SocketListener::closeConnections(LPVOID mParams)
{
	while (!MSGQUEUE->isExitFlag() or MSGQUEUE->hasClientMessage())
	{
		while (MSGQUEUE->hasClientMessage())
		{
			MessageQueue* q = MSGQUEUE;
			Messages::ClientMessage msg = MSGQUEUE->getClientMessage();
			ClientsList::iterator it = _clients.begin();
			while (it != _clients.end()) {
				SClientPtr client = *it;
				if (client.get() == msg.ptr)
				{
					Messages::Message send{ msg.type, 1, msg.text };
					MSGQUEUE->postMessage(send);
					it = _clients.erase(it);
				}
				else
					it++;
			}	
		}
	}
	return 0;
}

std::string SocketListener::getClientHost(SOCKADDR_IN addr) const
{
	int addrSize = sizeof(addr);
	char host[NI_MAXHOST];
	char port[NI_MAXSERV];

	ZeroMemory(host, NI_MAXHOST);
	ZeroMemory(port, NI_MAXSERV);

	if (!getnameinfo((SOCKADDR*)&addr, addrSize, host, NI_MAXHOST, port, NI_MAXSERV, 0) == 0)
	{
		inet_ntop(AF_INET, &addr.sin_addr, host, NI_MAXHOST);
		snprintf(port, NI_MAXSERV, "%d", ntohs(addr.sin_port));
	}
	return std::string(host);
}

std::string SocketListener::getClientPort(SOCKADDR_IN addr) const
{
	int addrSize = sizeof(addr);
	char host[NI_MAXHOST];
	char port[NI_MAXSERV];

	ZeroMemory(host, NI_MAXHOST);
	ZeroMemory(port, NI_MAXSERV);

	if (!getnameinfo((SOCKADDR*)&addr, addrSize, host, NI_MAXHOST, port, NI_MAXSERV, 0) == 0)
	{
		inet_ntop(AF_INET, &addr.sin_addr, host, NI_MAXHOST);
		snprintf(port, NI_MAXSERV, "%d", ntohs(addr.sin_port));
	}
	return std::string(port);
}
