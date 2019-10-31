#include <iostream>
#include "SocketListener.h"
#include "InputListener.h"
#include "Thread.h"
#include "MessageQueue.h"
#include "RequestWriter.h"
#include "namespaces.h"
#include <system_error>

#pragma warning( disable : 4244 )


INT main(int argc, char* argv[], char* envp[])
{
	setlocale(LC_ALL, "Russian");


	if (argc != 2)
	{
		std::cout << AppErrors::INVALID_PARAMETERS_NUMBER << std::endl;
		std::cout << AppMessages::PARAMETERS + std::string("Server.exe <path:>") << std::endl;
		exit(1);
	}
	
	std::cout << AppMessages::START << std::endl;
	
	SocketListener server;

	SocketListener::Params params;
	params.port = 18080;

	Thread<SocketListener> serverThread(&server, &SocketListener::start, &params);
	if (!serverThread.start())
	{
		std::cout << AppErrors::THREAD_RUNNING_ERROR << std::endl;
		exit(1);
	}

	Thread<SocketListener> closeClientsThread(&server, &SocketListener::closeConnections, NULL);
	if (!closeClientsThread.start())
	{
		std::cout << AppErrors::THREAD_RUNNING_ERROR << std::endl;
		exit(1);
	}

	WaitForSingleObject(server.initEvent(), INFINITY);
	WaitForSingleObject(server.createSocketEvent(), INFINITY);
	WaitForSingleObject(server.socketBindEvent(), INFINITY);

	InputListener input;

	Thread<InputListener> inputThread(&input, &InputListener::start, NULL);
	if (!inputThread.start())
	{
		std::cout << AppErrors::THREAD_RUNNING_ERROR << std::endl;
		exit(1);
	}

	RequestWriter writer;
	RequestWriter::Params rwParams;
	rwParams.path = std::string(argv[1]);

	Thread<RequestWriter> requestWriterThread(&writer, &RequestWriter::start, &rwParams);
	if (!requestWriterThread.start())
	{
		std::cout << AppErrors::THREAD_RUNNING_ERROR << std::endl;
		exit(1);
	}

	std::cout << AppMessages::SERVER_RUNNING << std::endl;

	while (!MSGQUEUE->isExitFlag())
	{
		while (MSGQUEUE->hasMessage())
		{
			Messages::Message msg = MSGQUEUE->getMessage();
			std::string text = msg.text;
			if (msg.type == Messages::ERRROR)
				text += std::string(": ") + std::system_category().message(msg.code);
			std::cout << text << std::endl;
		}	
	}

	CloseHandle(serverThread.getThreadHandle());
	CloseHandle(inputThread.getThreadHandle());

	serverThread.join();
	inputThread.join();

	return 0;
}