#pragma once
#include "SocketListener.h"
#include "Client.h"


class MockSocketListener : public SocketListener
{
public:
	void setClients(const ClientsList clients);
	ClientsList getClients() const;
};

