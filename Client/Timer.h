#pragma once
#include <Windows.h>

class Timer
{
public:
	struct Params
	{
		int delay;
	};
public:
	DWORD start(LPVOID mParams);
};

