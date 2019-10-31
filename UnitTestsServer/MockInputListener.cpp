#include "pch.h"
#include "MockInputListener.h"

void MockInputListener::setInput(const std::string text)
{
	this->_input = text;
}

std::string MockInputListener::getSTDIn()
{
	return this->_input;
}
