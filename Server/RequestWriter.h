#pragma once
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <strsafe.h>
#include <string>

class RequestWriter
{
public:
	struct Params {
		std::string path;
	};

public:
	DWORD start(LPVOID mParams);
	void write(std::string name, std::string text);
private:
	std::string _pathPrefix;
};

