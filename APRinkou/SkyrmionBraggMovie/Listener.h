
#pragma once

#include <gl\freeglut.h>
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
#pragma comment(lib,"cv210")
#pragma comment(lib,"cxcore210")
#pragma comment(lib,"highgui210")
#pragma comment(lib,"cvaux210")
#endif
#include <vector>
#include <complex>

using namespace std;

#define MAXLEN ((double)(10.0))
#define SIZE 20

class Listener{
private:
	bool isCheck;
	bool isVideo;
	bool isDraw;
	bool isEnd;

	char outfile[1024];
	char outvfile[1024];
	char* data;
	IplImage* img;

	double cnorm[SIZE*2+1][SIZE*2+1];
	complex <double> cval[SIZE*2+1][SIZE*2+1];
	double maxNorm;
	double qt;

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
	unsigned int frc;
	vector < int > frl;
	CvVideoWriter *video;

public:
	Listener(){
		isCheck=true;
		isVideo=false;
		camx=desx=0;
		camy=desy=0;
		camz=45;
		desz=0;
		kdz=0.5;
		kdxy=0.1;

		sp=1;
		dsp=1;
	}
	virtual bool Init(int,char**);
	virtual void Disp();
	virtual int Timer();
	virtual void CalcCurrent();
	virtual void Keyboard(unsigned char key);
};
