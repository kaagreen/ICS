#include "pch.h"
#include "MockSocketListener.h"

void MockSocketListener::setClients(const ClientsList clients)
{
	this->_clients = clients;
}

ClientsList MockSocketListener::getClients() const
{
	return this->_clients;
}
