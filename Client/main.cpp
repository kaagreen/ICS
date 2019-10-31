#include <iostream>
#include "Server.h"
#include "Timer.h"
#include "Thread.h"
#include "InputListener.h"
#include "MessageQueue.h"
#include "namespaces.h"

#pragma warning( disable : 4244 )

INT main(int argc, char* argv[], char* envp[])
{
	setlocale(LC_ALL, "Russian");

	std::cout << AppMessages::START << std::endl;

	if (argc != 3)
	{
		std::cout << AppErrors::INVALID_PARAMETERS_NUMBER << std::endl;
		std::cout << AppMessages::PARAMETERS + std::string("Client.exe <host> <timer_delay>") << std::endl;
		exit(1);
	}

	std::string ipAddress = std::string(argv[1]);;
	int port = 18080;
	int delay = atoi(argv[2]);

	InputListener input;

	Thread<InputListener> inputThread(&input, &InputListener::start, NULL);
	if (!inputThread.start())
	{
		std::cout << AppErrors::THREAD_RUNNING_ERROR << std::endl;
		exit(1);
	}
	
	Server connection;
	
	Timer timer;
	Timer::Params params{ delay };
	Thread<Timer> timerThread(&timer, &Timer::start, &params);

	connection.connectToServer(ipAddress, port);
	Thread<Server> serverThread(&connection, &Server::start, NULL);
	if (connection.connected())
	{
		if (!serverThread.start())
		{
			std::cout << AppErrors::THREAD_RUNNING_ERROR << std::endl;
			exit(1);
		}
		if (!timerThread.start())
		{
			std::cout << AppErrors::THREAD_RUNNING_ERROR << std::endl;
			exit(1);
		}

	}
	
	while (!MSGQUEUE->isExitFlag())
	{
		bool exit_f = false;
		while (MSGQUEUE->hasMessage())
		{
			Messages::Message msg = MSGQUEUE->getMessage();
			std::string text = msg.text;
			if (msg.type == Messages::ERRROR)
				text += std::string(": ") + std::system_category().message(msg.code);
			std::cout << text << std::endl;
			if (msg.type == Messages::ERRROR)
				exit_f = true;
		}
		if (exit_f)
		{
			Messages::Message error{ Messages::EXIT, 0, AppErrors::ERROR_EXIT };
			MSGQUEUE->postMessage(error);
		}
	}

	CloseHandle(serverThread.getThreadHandle());
	CloseHandle(timerThread.getThreadHandle());

	serverThread.join();
	timerThread.join();

	return 0;
}