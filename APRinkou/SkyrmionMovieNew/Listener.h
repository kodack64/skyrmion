
#pragma once

#include "GL\freeglut.h"
#include <cv.h>
#include <cvaux.h>
#include <highgui.h>
#include <cxcore.h>
#ifdef _DEBUG
#pragma comment(lib,"cv210d")
#pragma comment(lib,"cxcore210d")
#pragma comment(lib,"highgui210d")
#pragma comment(lib,"cvaux210d")
#else
#pragma comment(lib,"opencv_core220.lib")
#pragma comment(lib,"opencv_imgproc220.lib")
#pragma comment(lib,"opencv_highgui220.lib")
#endif
#include <vector>

using namespace std;

class Listener{
private:
	bool isCheck;
	bool isVideo;
	bool isDraw;
	bool isEnd;
	bool hasPot;

	char outfile[1024];
	char outpfile[1024];
	char outvfile[1024];
	char* data;
	IplImage* img;

	double camx;
	double camy;
	double camz;
	double desx;
	double desy;
	double desz;
	double kdz;
	double kdxy;

	int sp;
	int dsp;
	int drawCount;

	double dt;
	int n;
	vector < vector<double> > x;
	vector < vector<double> > y;
	vector < double > p;
	unsigned int frc;
	vector < int > frl;
	CvVideoWriter *video;

public:
	Listener(){
		isCheck=true;
		isVideo=false;
		camx=desx=0;
		camy=desy=0;
		camz=20;
		desz=0;
		kdz=0.5;
		kdxy=0.1;

		sp=1;
		dsp=1;
	}
	virtual bool Init(int,char**);
	virtual void Disp();
	virtual int Timer();
	virtual void Keyboard(unsigned char key);
};
