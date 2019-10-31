#pragma once
#include "singleton.h"
#include <queue>
#include <Windows.h>
#include <string>

#pragma warning( disable : 4244 )

namespace Messages {

	enum MsgTypes {
		ERRROR, NOTYFICATION, EXIT, CLOSE_CLIENT
	};

	struct Message {
		Message(MsgTypes type, DWORD code, std::string text);
		MsgTypes type;
		DWORD code;
		std::string text;
	};

	struct ClientMessage : public Message {
		ClientMessage(MsgTypes type, DWORD code, std::string text, LPVOID ptr);
		LPVOID ptr;
	};

	struct WFMessage {
		std::string name;
		std::string text;
	};
}

class MessageQueue
{
public:
	explicit MessageQueue();
	void postMessage(Messages::Message msg);
	void postClientMessage(Messages::ClientMessage msg);
	void postFWMessage(Messages::WFMessage msg);
	Messages::Message getMessage();
	Messages::ClientMessage getClientMessage();
	Messages::WFMessage getWFMessage();
	bool hasMessage() const;
	bool hasClientMessage() const;
	bool hasWFMessage() const;
	bool isExitFlag() const;
private:
	std::queue<Messages::Message>		_mainQueue;
	std::queue<Messages::ClientMessage>	_clientQueue;
	std::queue<Messages::WFMessage>		_WFQueue;
	bool								_exitFlag;
	HANDLE								_mainMessagesMutex;
	HANDLE								_clientMessagesMutex;
	HANDLE								_WFMessagesMutex;
};

#define MSGQUEUE Singleton<MessageQueue>::instance()

