#pragma once
#include <WS2tcpip.h>
#include <string>

#pragma comment (lib, "ws2_32.lib")

class Server
{
public:
	explicit Server();
	~Server();
	void connectToServer(const std::string host, const int port);
	DWORD start(LPVOID mParams);
	bool connected() const;
private:
	SOCKET	sSocket;
	bool	_connected;
};

