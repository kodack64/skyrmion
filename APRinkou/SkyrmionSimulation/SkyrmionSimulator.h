
#pragma once

#include "Simulator.h"

class SkyrmionSimulator : public Simulator{
private:
	//time
	double t;
	double dt;

	//pos vel for
	double vx[MAXN];
	double vy[MAXN];
	double vbx[MAXN];
	double vby[MAXN];
	double fx[MAXN];
	double fy[MAXN];

	//env
	double w;
	double h;
	double br;

	int borderShape;
	double borderPositionInterval;
	int squareBorderPositionType;
	int squareBorderLoopLR;
	int squareBorderLoopUD;
	int squareBorderSolidLR;
	int squareBorderSolidUD;
	int squareBorderPositionRowNumber;
	int squareBorderPositionColNumber;

	int circleBorderPositionType;//0 square , 1 tri , 2 circle
	int circleBorderSolid;
	int circleBorderPositionRadiusNumber;

	//rand
	int sr;
	double prand;

	//potential
	double psum;
	double psumf;


	//potential coef
	double pfv;
	double ve;
	double sigma;
	double sigma12;
	double sigma6;
	double cpotential;

	//frames
	int maxFrame;

	//cycle
	int borderStart;
	int borderEnd;
	int dif[9][2];

	//color
	double cr;
	double cg;
	double cb;
	int ballGraphicDivide;

	bool outputResultFlag;
	string outputFile;
	int outputResultInterval;
	int consolePrintResultInterval;

public:
	virtual int Step();
	virtual int Init();
	virtual int WriteData();

#ifdef _MSC_VER
	virtual int Draw();
#endif

private:
	virtual double drand();
	virtual void InitParameter();
	virtual void InitPosition();
	virtual void CalcForce();
	virtual void CalcForceCollision();
	virtual void CalcForceWall();
//	virtual void CalcForceField();
//	virtual pdVertex2d CalcInteractionForce(pdVertex2d r1,pdVertex2d r2);
	virtual void CalcInteractionForce(double,double,double,double,double*,double*);
	virtual void CalcVectorSkyrmion();
//	virtual void SetParam();
	virtual void putCircle(pdVertex2d,double);
};
