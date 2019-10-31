#pragma once
#include <Windows.h>
#include "json.hpp"
#include <string>
#include <list>
#include <memory>

using json = nlohmann::json;

struct Window;

typedef std::shared_ptr<Window> SWindowPtr;
typedef std::list<SWindowPtr> WindowsList;

struct Window
{
	std::string name;
	WindowsList children;
};

class Collector
{
public:
	explicit Collector();
	~Collector();
	void collect();

protected:
	virtual void getWindowsTree();

private:
	json _toJsonArray(WindowsList windows);

	static BOOL CALLBACK EnumWindowsCallback(HWND hwnd, LPARAM lParam);
	static BOOL CALLBACK EnumChildWindowCallback(HWND hwnd, LPARAM lParam);
	static std::string toUtf8(const std::string& str);

protected:
	WindowsList _collection;
};

