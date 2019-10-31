#pragma once
#include "namespaces.h"
#include <WinSock2.h>
#include <string>
#include "Client.h"
#pragma comment (lib, "ws2_32.lib")

class SocketListener
{
public:
	struct Params {
		u_short port = 18080;
	};

public:
	explicit SocketListener();
	~SocketListener();
	DWORD start(LPVOID mParams);
	DWORD closeConnections(LPVOID mParams);
	HANDLE initEvent() const;
	HANDLE createSocketEvent() const;
	HANDLE socketBindEvent() const;
	bool isSocketBinded() const;
	bool isSocketValid() const;

	SocketListener(const SocketListener& other) = delete;
	SocketListener& operator =(const SocketListener& other) = delete;

private:
	void acceptNewConnection();
	std::string getClientHost(SOCKADDR_IN addr) const;
	std::string getClientPort(SOCKADDR_IN addr) const;

protected:
	ClientsList				_clients;

private:
	SOCKET					sListen;
	SOCKADDR_IN				aListen;
	HANDLE					_initEvent;
	HANDLE					_createSocketEvent;
	HANDLE					_socketBindEvent;
	bool					_socketBinded;
};