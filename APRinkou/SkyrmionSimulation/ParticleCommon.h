
#pragma once

#include "Common.h"

//VERTEX

class pdVertex2d{
public:
	pdVertex2d(){x=y=0;}
	pdVertex2d(double p,double q){x=p;y=q;}
	double x;
	double y;

	pdVertex2d operator + (const pdVertex2d dvec) const {
		pdVertex2d v;
		v.x = x + dvec.x;
		v.y = y + dvec.y;
		return v;
	};

	pdVertex2d operator - (const pdVertex2d dvec) const {
		pdVertex2d v;
		v.x = x - dvec.x;
		v.y = y - dvec.y;
		return v;
	};

	pdVertex2d operator * (double d) const {
		pdVertex2d v;
		v.x = x * d;
		v.y = y * d;
		return v;
	};

	pdVertex2d operator / (double d) const {
		pdVertex2d v;
		if(d < EPS){
			return *this;
		}
		v.x = x / d;
		v.y = y / d;
		return v;
	};

	pdVertex2d& operator += (const pdVertex2d dvec) {
		x += dvec.x;
		y += dvec.y;
		return *this;
	};

	pdVertex2d& operator -= (const pdVertex2d dvec) {
		x -= dvec.x;
		y -= dvec.y;
		return *this;
	};

	pdVertex2d& operator *= (double d) {
		x *= d;
		y *= d;
		return *this;
	};

	pdVertex2d& operator /= (double d) {
		if(d < EPS){
			return *this;
		}
		x /= d;
		y /= d;
		return *this;
	};
	double length(){return sqrt(x * x + y * y);}
};

class pdVertex3d{
public:
	pdVertex3d(){x=y=z=0;}
	pdVertex3d(double p,double q,double r){x=p;y=q;z=r;}
	double x;
	double y;
	double z;
	pdVertex3d operator + (const pdVertex3d dvec) const {
		pdVertex3d v3;
		v3.x = x + dvec.x;
		v3.y = y + dvec.y;
		v3.z = z + dvec.z;
		return v3;
	};

	pdVertex3d operator - (const pdVertex3d dvec) const {
		pdVertex3d v3;
		v3.x = x - dvec.x;
		v3.y = y - dvec.y;
		v3.z = z - dvec.z;
		return v3;
	};

	pdVertex3d operator * (double d) const {
		pdVertex3d v3;
		v3.x = x * d;
		v3.y = y * d;
		v3.z = z * d;
		return v3;
	};

	pdVertex3d operator / (double d) const {
		pdVertex3d v3;
		if(d == 0.0){
			return v3;
		}
		v3.x = x / d;
		v3.y = y / d;
		v3.z = z / d;
		return v3;
	};

	pdVertex3d& operator += (const pdVertex3d dvec) {
		x += dvec.x;
		y += dvec.y;
		z += dvec.z;
		return *this;
	};

	pdVertex3d& operator -= (const pdVertex3d dvec) {
		x -= dvec.x;
		y -= dvec.y;
		z -= dvec.z;
		return *this;
	};

	pdVertex3d& operator *= (double d) {
		x *= d;
		y *= d;
		z *= d;
		return *this;
	};
	pdVertex3d& operator /= (double d) {
		if(d == 0.0){
			return *this;
		}

		x /= d;
		y /= d;
		z /= d;
		return *this;
	};
	double length(){return sqrt(x * x + y * y + z * z);}
};

pdVertex3d pdVecCross (pdVertex3d p , pdVertex3d q);
pdVertex2d pdVecCross (pdVertex2d p , pdVertex2d q);
pdVertex3d pdVecDot (pdVertex3d p , pdVertex3d q);
pdVertex2d pdVecDot (pdVertex2d p , pdVertex2d q);


//COLORS
class pdColor{
public:
	pdColor(){r=g=b=0.0;a=1.0;}
	pdColor(double rd,double gd,double bd){r=rd;g=gd;b=bd;a=1.0;}
	pdColor(double rd,double gd,double bd,double ad){r=rd;g=gd;b=bd;a=ad;}
	double r;
	double g;
	double b;
	double a;
};

/*
//PARAM
class pdParam{
public:
	pdParam(){
		dataout=false;
	}
	bool dataout;
	pdColor c;
	double mass;
};
struct pdParam2d:public pdParam{
public:
	pdVertex2d r;
	pdVertex2d v;
	pdVertex2d rot;
};
struct pdParam3d:public pdParam{
public:
	pdVertex3d r;
	pdVertex3d v;
	pdVertex3d rot;
};


//SHAPES

class pdShape{
public:
	virtual int GetMyDim()=0;
};
class pdShape2d:public pdShape{
public:
#ifdef _MSC_VER
	virtual int Draw(pdParam2d)=0;
#endif
	virtual int GetMyDim(){return 2;}
};
class pdShape3d:public pdShape{
public:
#ifdef _MSC_VER
	virtual int Draw(pdParam3d)=0;
#endif
	virtual int GetMyDim(){return 3;}
};
class pdCircle2d:public pdShape2d{
private:
	double r;
public:
	virtual int Init(double r);
	virtual double GetR(){return r;};
#ifdef _MSC_VER
	virtual int Draw(pdParam2d p);
#endif
};
class pdCircle3d:public pdShape3d{
private:
	double r;
public:
	virtual int Init(double r);
	virtual double GetR(){return r;};
#ifdef _MSC_VER
	virtual int Draw(pdParam3d p);
#endif
};


// BODY
class pdBody{
private:
	int myDim;
	pdShape* myShape;
	pdParam2d param2d;
	pdParam3d param3d;
public:
	pdBody(pdShape* shape){
		myShape=shape;
		myDim=shape->GetMyDim();
	};
	virtual ~pdBody(){
		SAFE_DELETE(myShape);
	}
	virtual pdShape* GetShape(){
		return myShape;
	}
#ifdef _MSC_VER
	virtual int Draw(){
		if(myDim==2){
			((pdShape2d*)myShape)->Draw(param2d);
		}else{
			((pdShape3d*)myShape)->Draw(param3d);
		}
		return 0;
	}
#endif
	virtual int GetDimention(){return myDim;}
	virtual int SetParam(pdParam2d p){
		param2d=p;
		if(myDim==3)
			printf("this is 3D object ,but is SetParam as 2D object.\n");
		return 0;
	};
	virtual int SetParam(pdParam3d p){
		param3d=p;
		if(myDim==2)
			printf("this is 2D object ,but is SetParam as 3D object.\n");
		return 0;
	};
	virtual pdParam2d GetParam2d(){
		if(myDim==3)
			printf("this is 3D object ,but you request 2D parameter.\n");
		return param2d;
	}
	virtual pdParam3d GetParam3d(){
		if(myDim==2)
			printf("this is 2D object ,but you request 3D parameter.\n");
		return param3d;
	}
};
*/
/* preset function*/

#ifdef _MSC_VER
//Screen
int pdClearScreen();
int pdSetScreenColor(pdColor);
int pdSwapScreen();

//Setup
int pdInit();
static int pdBallDivide;
static int pdSphereDivideX;
static int pdSphereDivideY;
static bool pdUseLight;
#endif

//Draw
#ifdef _MSC_VER
int pdDrawLine(pdVertex2d*,pdColor*,int num);
int pdDrawLine(pdVertex3d*,pdColor*,int num);
int pdDrawLine(vector<pdVertex2d>,vector<pdColor>);
int pdDrawLine(vector<pdVertex3d>,vector<pdColor>);
int pdDrawPolygon(pdVertex2d*,pdColor*,int num);
int pdDrawPolygon(pdVertex3d*,pdColor*,int num);
int pdDrawPolygon(vector<pdVertex2d>,vector<pdColor>);
int pdDrawPolygon(vector<pdVertex3d>,vector<pdColor>);
int pdDrawCircle(pdVertex2d,pdColor,double r);
int pdDrawCircle(pdVertex3d,pdColor,double r);
int pdDrawBall(pdVertex2d,pdColor,double r);
int pdDrawBall(pdVertex3d,pdColor,double r);
//int pdDrawBody(pdBody*);
int pdDrawString(const char *str,pdVertex3d,pdColor);
int pdDrawString(const char *str,pdVertex2d,pdColor);
int pdDrawString(string,pdVertex3d,pdColor);
int pdDrawString(string,pdVertex2d,pdColor);
#else
#define pdDrawLine(p,q,r)
//#define pdDrawLine(p,q)
#define pdDrawPolygon(p,q,r)
//#define pdDrawPolygon(p,q)
#define pdDrawCircle(p,q,r)
#define pdDrawBall(p,q,r)
#define pdDrawBall(p,q,r)
#define pdDrawBody(p)
#define pdDrawString(p,q,r)
#endif

