#include "InputListener.h"
#include "MessageQueue.h"
#include <string>
#include "namespaces.h"

DWORD InputListener::start(LPVOID mParams)
{
	while (!MSGQUEUE->isExitFlag())
	{
		std::string str;
		std::cin >> str;
		std::transform(str.begin(), str.end(), str.begin(),
			[](TCHAR c) { return std::tolower(c); });

		if (str == "exit")
		{
			Messages::Message msg{ Messages::EXIT, 0, AppMessages::EXIT };
			MSGQUEUE->postMessage(msg);
		}
	}
	return 0;
}
