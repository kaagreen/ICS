#pragma once
#include "singleton.h"
#include <queue>
#include <Windows.h>
#include <string>

#pragma warning( disable : 4244 )

namespace Messages {

	enum MsgTypes {
		ERRROR, NOTYFICATION, EXIT
	};

	struct Message {
		Message(MsgTypes type, DWORD code, std::string text);
		MsgTypes type;
		DWORD code;
		std::string text;
	};
}

class MessageQueue
{
public:
	explicit MessageQueue();
	void postMessage(Messages::Message msg);
	void postServerMessage(Messages::Message msg);
	Messages::Message getMessage();
	Messages::Message getServerMessage();
	bool hasMessage() const;
	bool hasServerMessage() const;
	bool isExitFlag() const;
private:
	std::queue<Messages::Message>		_mainQueue;
	std::queue<Messages::Message>		_serverQueue;
	bool								_exitFlag;
	HANDLE								_mainMessagesMutex;
	HANDLE								_serverMessagesMutex;
};

#define MSGQUEUE Singleton<MessageQueue>::instance()