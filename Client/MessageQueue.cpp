#include "MessageQueue.h"

MessageQueue::MessageQueue() : _exitFlag(false)
{
	this->_mainMessagesMutex = CreateMutex(NULL, FALSE, NULL);
	this->_serverMessagesMutex = CreateMutex(NULL, FALSE, NULL);
}

void MessageQueue::postMessage(Messages::Message msg)
{	
	WaitForSingleObject(this->_mainMessagesMutex, INFINITY);
	_mainQueue.push(msg);
	if (msg.type == Messages::EXIT)
		_exitFlag = true;
	ReleaseMutex(this->_mainMessagesMutex);
}

void MessageQueue::postServerMessage(Messages::Message msg)
{
	WaitForSingleObject(this->_serverMessagesMutex, INFINITY);
	_serverQueue.push(msg);
	ReleaseMutex(this->_serverMessagesMutex);
}

Messages::Message MessageQueue::getMessage()
{
	WaitForSingleObject(this->_mainMessagesMutex, INFINITY);
	Messages::Message msg = _mainQueue.front();
	_mainQueue.pop();
	ReleaseMutex(this->_mainMessagesMutex);
	return msg;
}

Messages::Message MessageQueue::getServerMessage()
{
	WaitForSingleObject(this->_serverMessagesMutex, INFINITY);
	Messages::Message msg = _serverQueue.front();
	_serverQueue.pop();
	ReleaseMutex(this->_serverMessagesMutex);
	return msg;
}

bool MessageQueue::hasMessage() const
{
	return !_mainQueue.empty();
}

bool MessageQueue::hasServerMessage() const
{
	return !_serverQueue.empty();
}

bool MessageQueue::isExitFlag() const
{
	return _exitFlag;
}

Messages::Message::Message(MsgTypes type, DWORD code, std::string text) :
	type(type),
	code(code),
	text(text)
{}