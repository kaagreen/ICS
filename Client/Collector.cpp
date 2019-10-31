#include "Collector.h"
#include "MessageQueue.h"

Collector::Collector()
{}

Collector::~Collector()
{}

void Collector::collect()
{
	getWindowsTree();

	json root = json::object();
	root["windows"] = _toJsonArray(this->_collection);
		
	try
	{
		std::string str = root.dump();
		Messages::Message msg{ Messages::NOTYFICATION, 0, str };
		MSGQUEUE->postServerMessage(msg);
	}
	catch (const std::exception & e)
	{
		Messages::Message msg{ Messages::NOTYFICATION, 0, TEXT(e.what()) };
		MSGQUEUE->postMessage(msg);
	}
}

void Collector::getWindowsTree()
{
	this->_collection.clear();
	EnumWindows(EnumWindowsCallback, (LPARAM)(&this->_collection));
}

json Collector::_toJsonArray(WindowsList windows)
{
	json root;
	if (windows.empty())
		return NULL;
	for (SWindowPtr window : windows)
	{
		json object = json::object();
		object["name"] = window->name;
		object["children"] = _toJsonArray(window->children);
		root.push_back(object);
	}
	return root;
}

BOOL Collector::EnumWindowsCallback(HWND hwnd, LPARAM lParam)
{
	WindowsList *windows = (WindowsList*)lParam;
	CHAR name[255];
	if (GetWindowText(hwnd, name, 255))
	{
		if (IsWindowVisible(hwnd) && (!GetWindow(hwnd, GW_OWNER)))
		{
			SWindowPtr window = std::make_shared<Window>();
			window->name = Collector::toUtf8(std::string(name));
			EnumChildWindows(hwnd, EnumWindowsCallback, (LPARAM)(&window->children));
			windows->push_back(window);
		}
	}
	return 1;
}

BOOL Collector::EnumChildWindowCallback(HWND hwnd, LPARAM lParam)
{
	return 0;
}

std::string Collector::toUtf8(const std::string& str)
{
	int size = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, 0, 0);
	std::wstring wstr;
	if (size > 0)
	{
		std::vector<WCHAR> buffer(size);
		MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, &buffer[0], size);
		wstr.assign(buffer.begin(), buffer.end() - 1);
	}

	std::string convertedString;
	int requiredSize = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, 0, 0, 0, 0);
	if (requiredSize > 0)
	{
		std::vector<char> buffer(requiredSize);
		WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &buffer[0], requiredSize, 0, 0);
		convertedString.assign(buffer.begin(), buffer.end() - 1);
	}
	return convertedString;
}
