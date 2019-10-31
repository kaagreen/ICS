#include "Server.h"
#include "MessageQueue.h"
#include "namespaces.h"

Server::Server() : _connected(false)
{
	WSADATA data;
	WORD ver = MAKEWORD(2, 2);
	int initResult = WSAStartup(ver, &data);
	if (initResult != 0)
	{
		Messages::Message msg{Messages::ERRROR, GetLastError(), AppErrors::WSA_INIT_ERROR };
		MSGQUEUE->postMessage(msg);
	}

	this->sSocket = socket(AF_INET, SOCK_STREAM, 0);

	if (this->sSocket == INVALID_SOCKET)
	{
		Messages::Message msg{ Messages::ERRROR, GetLastError(), AppErrors::SOCKET_INIT_ERROR };
		MSGQUEUE->postMessage(msg);
	}
}

Server::~Server()
{
	closesocket(this->sSocket);
	WSACleanup();
}

void Server::connectToServer(const std::string host, const int port)
{
	int connResult = SOCKET_ERROR;
	SOCKADDR_IN addr;
	int addrSize = sizeof(addr);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	int getaddrresult = -1;
	if (!inet_pton(AF_INET, host.c_str(), &addr.sin_addr))
	{
		struct addrinfo hints, *res;
		ZeroMemory(&hints, sizeof(hints));
		getaddrresult = getaddrinfo(host.c_str(), std::to_string(port).c_str(), &hints, &res);
		if (getaddrresult == 0)
		{
			struct addrinfo* ptr = NULL;
			for (ptr = res; ptr != NULL; ptr = ptr->ai_next)
			{
				if (ptr->ai_family == AF_INET)
				{
					connResult = connect(this->sSocket, ptr->ai_addr, ptr->ai_addrlen);
					break;
				}
			}
		}
	}
	else
		connResult = connect(this->sSocket, (sockaddr*)&addr, addrSize);

	if (connResult == SOCKET_ERROR)
	{
		Messages::Message msg{ Messages::ERRROR, GetLastError(), AppErrors::CONNECTION_CLOSED };
		MSGQUEUE->postMessage(msg);
		_connected = false;
		return;
	}
	else _connected = true;
}

DWORD Server::start(LPVOID mParams)
{
	while (!MSGQUEUE->isExitFlag() && _connected)
	{
		if (this->sSocket == INVALID_SOCKET)
		{
			Messages::Message msg{ Messages::ERRROR, GetLastError(), AppErrors::CONNECTION_CLOSED };
			MSGQUEUE->postMessage(msg);
			_connected = false;
			break;
		}
		while (MSGQUEUE->hasServerMessage())
		{
			Messages::Message msg = MSGQUEUE->getServerMessage();
			int size = msg.text.size();
			int sendResult = send(this->sSocket, (char*)&size, sizeof(int), 0);
			sendResult = send(this->sSocket, msg.text.c_str(), msg.text.size(), 0);

			if (sendResult == SOCKET_ERROR)
			{
				Messages::Message msg{ Messages::ERRROR, GetLastError(), AppErrors::CONNECTION_CLOSED };
				MSGQUEUE->postMessage(msg);
			}
		}
	}
	return 0;
}

bool Server::connected() const
{
	return _connected;
}
