#include "Timer.h"
#include "MessageQueue.h"
#include <ctime>
#include "Collector.h"

DWORD Timer::start(LPVOID mParams)
{
	Timer::Params *params = (Timer::Params*)mParams;
	clock_t t1 = clock();
	while (!MSGQUEUE->isExitFlag())
	{
		clock_t t2 = clock();
		if ((t2 - t1) / CLOCKS_PER_SEC > params->delay)
		{
			Collector worker;
			worker.collect();
			t1 = clock();
		}
	}
	return 0;
}
