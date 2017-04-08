
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <cassert>
#include <cstring>
#include <string>
#include <iostream>

const double PI = (2*acos(0.0));
const double ve=1.0;
const double sigma=pow(2,-1.0/6);
const double sigma12 = pow(sigma,12);
const double sigma6 = pow(sigma,6);
const double eps=1e-10;
const double randdiv=0.0f;

using namespace std;

#ifdef _MSC_VER
#pragma warning(disable:4996)
#endif


int locate(int nl,double* x,double* y,double wd){
	double hd=wd*sin(PI/3);
	int i,j;
	int n=0;
	int center=0;
	double di;
	for(j=-nl+1;j<=nl-1;j++){
		if(abs(j)%2==0){
			center=nl-1-abs(j)/2;
		}else{
			center=nl-2-(abs(j)+1)/2;
		}
		for(i=0;i<2*nl-1-abs(j);i++){
			if(abs(j)%2==0){
				di=(-nl+1+abs(j)/2+i);
			}else{
				di=(-nl+1+(abs(j)+1)/2+i-0.5);
			}
			x[n]=wd*di;
			y[n]=hd*j;
			n++;
		}
	}
	return n;
}

int main(int argc,char** argv){
	int nl;
	int n;
	double* x;
	double* y;
	double* fx;
	double* fy;
	double fpot;

	nl=4;

	/* setup */
	n = 3*nl*nl - 3*nl + 1;
	x = new double[n];
	y = new double[n];
	fx = new double[n];
	fy = new double[n];

	double up=1.1,bt=0.9;
	double div,mindiv;
	double minp=1000000;
	double minpp=10000000;
	double mindivp;
	for(div=bt;div<up;div+=0.0001){
		/* locate */
		if(n!=locate(nl,x,y,div)){
//			printf("incorrect location\n");return 0;
		}
		/* calc first potential */
		fpot=0;
		for(int i=0;i<n;i++){
			double mydis;
			double mydis6,mylen;
			fx[i]=0;
			fy[i]=0;
			for(int j=0;j<n;j++){
				if(i==j)continue;
				double dis6=pow(pow(x[i]-x[j],2)+pow(y[i]-y[j],2),-3);
				fpot+=4*ve*(sigma12*dis6-sigma6)*dis6;

				mydis=hypot(x[i]-x[j],y[i]-y[j]);
				if(mydis<eps)mydis=eps;
				mydis6=pow(mydis,-6);
				mylen=24*ve*( 2*sigma12*mydis6 - sigma6 )*mydis6/mydis;
				fx[i]+=-mylen*(x[i]-x[j])/mydis;
				fy[i]+=-mylen*(y[i]-y[j])/mydis;
			}
		}
		if(minp>fpot){
			minp=fpot;
			mindiv=div;
		}
		double sum=0;
		for(int i=0;i<n;i++){
			sum+=hypot(fx[i],fy[i]);
		}
		if(sum<minpp){
			minpp=sum;
			mindivp=div;
		}
		cout << div << " " << fpot << " " << sum << endl;
	}
//	cout <<"min : "<< mindiv << " " << minp << endl;
//	cout <<"minp : "<< mindivp << " " << minpp << endl;

	delete []x;
	delete []y;
//	printf("[Close]\n");
	return 0;
}