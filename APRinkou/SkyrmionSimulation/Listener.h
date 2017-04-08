
#pragma once

#include "Common.h"
#include "Simulator.h"
#include "ParticleCommon.h"

class MouseState{
public:
	MouseState(){
		mbr=mbl=mbm=ombr=ombl=ombm=ox=oy=0;
	}
	virtual ~MouseState(){};
	int mbr;
	int mbl;
	int mbm;
	int ombr;
	int ombl;
	int ombm;
	int ox;
	int oy;
};

class Camera{
public:
	double theta;
	double phi;
	double speed;
	double scale;
	pdVertex3d o;
	pdVertex3d eye;
	Camera(){
		theta=0;
		phi=0;
		eye.z=20.0;
		speed=0.01;
		scale=1.1;
	}
	virtual double GetDist(){
		return sqrt(pow(eye.x-o.x,2)+pow(eye.y-o.y,2)+pow(eye.z-o.z,2));
	}
	virtual void PolarToVertex(){
		double r = GetDist();
		eye.x=o.x+r*sin(theta)*cos(phi);
		eye.y=o.y+r*sin(theta)*sin(phi);
		eye.z=o.z+r*cos(theta);
	}
	virtual void VertexToPolar(){
		double r = GetDist();
		theta = acos((eye.z-o.z)/r);
		phi = acos((eye.x-o.x)/(r*sin(theta)));
	}
};

class Listener{
private:
	static Listener* listener;
	Simulator *sim;
	MouseState m;
	Camera cam;

	double stepFps;
	double drawFps;
	double logFps;
	int fpsCount;

	int lastElapsedTime;
	int elapsedFrame;

	int graphicCallCount;
	bool enableGraphic;

	bool fullscreen;
	int windowPosX;
	int windowPosY;
	int windowSizeX;
	int windowSizeY;

public:

	static Listener* GetInstance(){
		if(listener==NULL)listener=new Listener;
		return listener;
	};

	int Init(Simulator*);
	static void __cdecl _Timer(int val);
	int Timer();
	static void __cdecl _Disp(void);
	int Draw();
	static void __cdecl _Reshape(int w,int h);
	static void __cdecl _Mouse(int b,int st,int x,int y);
	int Mouse(int,int,int,int);
	static void __cdecl _Keyboard(unsigned char key,int x,int y);
	int Keyboard(unsigned char);
};
