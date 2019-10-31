#pragma once
#include "InputListener.h"

class MockInputListener : public InputListener
{
public:
	void setInput(const std::string text);
protected:
	virtual std::string getSTDIn() override;
private:
	std::string _input;
};

