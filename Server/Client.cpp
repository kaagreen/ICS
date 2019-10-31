#include "Client.h"
#include <string>
#include "MessageQueue.h"
#include <fstream>
#include "namespaces.h"

Client::Client(SOCKET socket, std::string name) :
	_name(name),
	cSocket(socket),
	threadPtr(NULL)
{}

Client::~Client()
{
	closesocket(cSocket);
	delete threadPtr;
}

DWORD Client::start(LPVOID mParams)
{
	int result = 1;

	while (result != 0)
	{
		int msg_size;
		
		result = recv(this->cSocket, (char*)&msg_size, sizeof(int), NULL);
		if (!validResult(result)) break;
		
		char* buff = new char[msg_size + 1];
		result = recv(this->cSocket, buff, msg_size, 0);
		buff[msg_size] = '\0';
		
		if (!validResult(result)) break;
		std::string request = std::string(buff, 0, result);
		
		Messages::WFMessage wf{ _name, request };
		MSGQUEUE->postFWMessage(wf);

		Messages::Message msg { Messages::NOTYFICATION, 0, AppMessages::REQUEST_RECEIVED + this->name()};
		MSGQUEUE->postMessage(msg);
		delete[] buff;
	}

	closesocket(this->cSocket);
	return 0;
}

void Client::setThreadHandle(Thread<Client> * ptr)
{
	this->threadPtr = ptr;
}

std::string Client::name() const
{
	return this->_name;
}

bool Client::validResult(const int result)
{
	if (result == SOCKET_ERROR)
	{
		Messages::ClientMessage msg{ Messages::ERRROR, GetLastError(), AppErrors::CONNECTION_CLOSED, this };
		MSGQUEUE->postClientMessage(msg);
		return false;
	}

	if (result == 0)
	{
		Messages::ClientMessage msg{ Messages::NOTYFICATION, 0, AppErrors::CONNECTION_CLOSED, this };
		MSGQUEUE->postClientMessage(msg);
		return false;
	}
	return true;
}
