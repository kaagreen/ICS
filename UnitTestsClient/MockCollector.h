#pragma once
#include "Collector.h"

class MockCollector : public Collector
{
public:
	std::string jsonStr() const;
protected:
	void getWindowsTree() override;
};

