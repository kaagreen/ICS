#include "RequestWriter.h"
#include "MessageQueue.h"
#include <iostream>
#include "namespaces.h"

DWORD RequestWriter::start(LPVOID mParams)
{
	Params* params = (Params*)mParams;
	this->_pathPrefix = params->path;
	while (!MSGQUEUE->isExitFlag())
	{
		while (MSGQUEUE->hasWFMessage())
		{
			Messages::WFMessage msg = MSGQUEUE->getWFMessage();
			write(msg.name, msg.text);
		}
	}

	return 0;
}

void RequestWriter::write(std::string name, std::string text)
{
	HANDLE hFile;
	const char* DataBuffer = text.c_str();
	DWORD dwBytesToWrite = (DWORD)strlen(DataBuffer);
	DWORD dwBytesWritten = 0;
	BOOL bErrorFlag = FALSE;
	std::string filename_str = this->_pathPrefix + "//" + name + ".json";
	LPSTR filename = const_cast<char*>(filename_str.c_str());
	hFile = CreateFile(filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		Messages::Message msg{Messages::ERRROR, GetLastError(), AppErrors::OPEN_FILE_ERROR + TEXT(" ") + filename_str };
		MSGQUEUE->postMessage(msg);
		return;
	}
	
	bErrorFlag = WriteFile(hFile, DataBuffer, dwBytesToWrite, &dwBytesWritten, NULL);
	if (FALSE == bErrorFlag)
	{
		Messages::Message msg{ Messages::ERRROR, GetLastError(), AppErrors::WRITE_FILE_ERROR };
		MSGQUEUE->postMessage(msg);
	}
	else
	{
		if (dwBytesWritten != dwBytesToWrite)
		{
			Messages::Message msg{ Messages::ERRROR, GetLastError(), AppErrors::WRITE_FILE_ERROR };
			MSGQUEUE->postMessage(msg);
		}
	}

	CloseHandle(hFile);
}
