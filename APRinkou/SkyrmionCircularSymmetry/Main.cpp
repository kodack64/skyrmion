
#include <string>
#include <cstdio>
#include <vector>
#include <Windows.h>
#include <iostream>
#include <cmath>
using namespace std;

#ifdef _MSC_VER
#pragma warning(disable:4996)
#endif

int main(int argc,char** argv){
	if(argc<=1){printf("no argument\n");return false;}

	char outfile[1024];
	char cirfile[1024];
	char gfile[1024];
	string str=argv[1];
	sprintf(outfile,"%s",str.c_str());
	sprintf(cirfile,"%s.cir",str.substr(0,str.find_first_of(".")).c_str());
	sprintf(gfile,"%s_circular.eps",str.substr(0,str.find_first_of(".")).c_str());

	char cdir[1024];
	GetCurrentDirectory(1024,cdir);

	printf("[Setup]\ncurrentdir=%s\ndatafile=%s\ncirfile=%s\ncirgraphfile=%s\n",cdir,outfile,cirfile,gfile);

	FILE *fp,*fpp;
	fp=fopen(outfile,"r");
	if(fp==NULL){printf("out file not found\n");return false;}
	fpp=fopen(cirfile,"w");
	if(fpp==NULL){printf("cir file cannot open\n");return false;}

	bool isCorrect=true;
	char buf[1024];
	int n;
	double dt;

	fgets(buf,1024,fp);
	isCorrect=isCorrect&(sscanf_s(buf,"dt = %lf",&dt)>0);
	if(dt==0){
		printf("dt is too small dt=1e-8 instead");
		dt=1e-8;
	}
	fgets(buf,1024,fp);
	isCorrect=isCorrect&(sscanf_s(buf,"n = %d",&n)>0);


	vector <vector <double> > x;
	vector <vector <double> > y;
	vector <int> fr;

	int dfr;
	int di;
	double px,py;
	printf("[LoadStart]\n");
	while(isCorrect){
		fgets(buf,1024,fp);
		if(sscanf_s(buf,"# %d",&dfr)<1)break;

		vector <double> tx;
		vector <double> ty;
		tx.resize(n,0);
		ty.resize(n,0);
		for(int i=0;i<n && isCorrect;i++){
			fgets(buf,1024,fp);
			isCorrect=isCorrect&(sscanf_s(buf,"%d %lf %lf",&di,&px,&py)>=3);
			if(di>=n){
				isCorrect=false;
			}else{
				tx[di]=px;
				ty[di]=py;
			}
		}
		if(isCorrect){
			x.push_back(tx);
			y.push_back(ty);
			fr.push_back(dfr);
		}
	}
	printf("[LoadEnd]\nframelength=%d\ntimelength=%lf\n",fr.size(),dt*fr[fr.size()-1]);
	fclose(fp);

	double real;
	double img;
	double theta;
	double L=(0.5+sqrt((double)(12*n-3))/6.0-1)*2;
	printf("[CalcCircularSymmetryStart]\n");
	fprintf(fpp,"# L = %lf\n",L);
	for(unsigned int f=0;f<fr.size();f++){
		real=img=0;
		for(int i=0;i<n;i++){
			theta=atan2(y[f][i],x[f][i]);
			real+=cos(6*theta);
			img+=sin(6*theta);
		}
		fprintf(fpp,"%d %lf %lf\n",fr[f],dt*fr[f],(real*real+img*img)/(L*L));
	}
	fclose(fpp);
	printf("[CalcCircularSymmetryEnd]\n");

	printf("[GraphStart]\n");
	fp = _popen("gnuplot","w");
	if(fp==NULL){printf("cannot open gnuplot\n");return 0;}
	fputs("set xlabel 'time(sec)'\n",fp);
	fputs("set ylabel 'I/(L^2)'\n",fp);
	fputs("set xrange [0:*]\n",fp);
	fputs("set yrange [*:*]\n",fp);

	fprintf(fp,"plot '%s' using 2:3 with lines ti 'I(%lf)'\n",cirfile,L);
	fputs("set terminal postscript enhanced\n",fp);
	fprintf(fp,"set output '%s'\n",gfile);
	fputs("replot",fp);
	fflush(fp);
	printf("push any key to exit");
	cin.get();
	_pclose(fp);
	printf("[GraphEnd]\n");
	return true;
}
