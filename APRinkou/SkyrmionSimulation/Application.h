
#pragma once

#include "Common.h"
#include "Simulator.h"

#ifdef _MSC_VER
#include "Listener.h"
#endif

class Application{
private:
	Simulator* sim;
public:
	virtual int Init();
	virtual int Close();
	virtual int Run();
};

