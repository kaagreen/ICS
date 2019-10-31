#include "pch.h"
#include "MockCollector.h"

std::string MockCollector::jsonStr() const
{
	return std::string("{\"windows\":[{\"children\":[{\"children\":0,\"name\":\"Child windows\"}],\"name\":\"Root window\"}]}");
}

void MockCollector::getWindowsTree()
{
	SWindowPtr rootWindow = std::make_shared<Window>();
	rootWindow->name = "Root window";
	SWindowPtr childWindow = std::make_shared<Window>();
	childWindow->name = "Child windows";
	rootWindow->children.push_back(childWindow);
	this->_collection.push_back(rootWindow);
}
