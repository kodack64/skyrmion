
#pragma once

#include "ParticleCommon.h"

#define MAXN 1000000

class Simulator{
protected:
	long frameCount;
	int n;
	double x[MAXN];
	double y[MAXN];
public:
	Simulator(){
		frameCount=0;
		n=0;
	}
	virtual ~Simulator(){
	}
	virtual int Init()=0;
	virtual int Step()=0;
	virtual int WriteData()=0;
#ifdef _MSC_VER
	virtual int Draw()=0;
#endif

	virtual long GetFrameCount(){return frameCount;}
	virtual unsigned int GetBodyNum(){
		return n;
	}
};
