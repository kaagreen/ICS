#include "MessageQueue.h"

MessageQueue::MessageQueue() : _exitFlag(false)
{
	this->_mainMessagesMutex = CreateMutex(NULL, FALSE, NULL);
	this->_clientMessagesMutex = CreateMutex(NULL, FALSE, NULL);
	this->_WFMessagesMutex = CreateMutex(NULL, FALSE, NULL);
}

void MessageQueue::postMessage(Messages::Message msg)
{	
	WaitForSingleObject(this->_mainMessagesMutex, INFINITY);
	_mainQueue.push(msg);
	if (msg.type == Messages::EXIT)
		_exitFlag = true;
	ReleaseMutex(this->_mainMessagesMutex);
}

void MessageQueue::postClientMessage(Messages::ClientMessage msg)
{
	WaitForSingleObject(this->_clientMessagesMutex, INFINITY);
	_clientQueue.push(msg);
	ReleaseMutex(this->_clientMessagesMutex);
}

void MessageQueue::postFWMessage(Messages::WFMessage msg)
{
	WaitForSingleObject(this->_WFMessagesMutex, INFINITY);
	_WFQueue.push(msg);
	ReleaseMutex(this->_WFMessagesMutex);
}

Messages::Message MessageQueue::getMessage()
{
	WaitForSingleObject(this->_mainMessagesMutex, INFINITY);
	Messages::Message msg = _mainQueue.front();
	_mainQueue.pop();
	ReleaseMutex(this->_mainMessagesMutex);
	return msg;
}

Messages::ClientMessage MessageQueue::getClientMessage()
{
	WaitForSingleObject(this->_clientMessagesMutex, INFINITY);
	Messages::ClientMessage msg = _clientQueue.front();
	_clientQueue.pop();
	ReleaseMutex(this->_clientMessagesMutex);
	return msg;
}

Messages::WFMessage MessageQueue::getWFMessage()
{
	WaitForSingleObject(this->_WFMessagesMutex, INFINITY);
	Messages::WFMessage msg = _WFQueue.front();
	_WFQueue.pop();
	ReleaseMutex(this->_WFMessagesMutex);
	return msg;
}

bool MessageQueue::hasMessage() const
{
	return !_mainQueue.empty();
}

bool MessageQueue::hasClientMessage() const
{
	return !_clientQueue.empty();
}

bool MessageQueue::hasWFMessage() const
{
	return !_WFQueue.empty();
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

Messages::ClientMessage::ClientMessage(MsgTypes type, DWORD code, std::string text, LPVOID ptr) :
	Messages::ClientMessage::Message(type,code,text),
	ptr(ptr)
{}
