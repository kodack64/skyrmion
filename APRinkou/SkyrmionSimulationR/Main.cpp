
#ifdef _OPENMP
#define USE_OPENMP
#endif

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <cassert>
#include <cstring>
#include <string>

#ifdef USE_OPENMP
#include <omp.h>
#endif USE_OPENMP

const double PI = (2*acos(0.0));
const double ve=1.0;
const double sigma=pow(2,-1.0/6);
const double sigma12 = pow(sigma,12);
const double sigma6 = pow(sigma,6);
const double wd=1.1;
//const double wd=0.9965;
const double hd=wd*sin(PI/3);
const double eps=1e-10;
const double randdiv=0.0f;

using namespace std;

#ifdef _MSC_VER
#pragma warning(disable:4996)
#endif

bool load(const char* setup,int* nl,double* mt,double* dt,double* r,int* photonum,char* file,char* potfile){
	FILE *fp;
	bool isLoaded=true;
	char buf[1024];
	char dummy[1024];
	fp=fopen(setup,"r");
	if(fp==NULL){printf("cannot open setup file\n");return false;}
	fgets(buf,1024,fp);
	isLoaded=isLoaded&(sscanf(buf,"%d %s",nl,dummy)>0);
	fgets(buf,1024,fp);
	isLoaded=isLoaded&(sscanf(buf,"%lf %s",mt,dummy)>0);
	fgets(buf,1024,fp);

	isLoaded=isLoaded&(sscanf(buf,"%lf %s",dt,dummy)>0);
	fgets(buf,1024,fp);
	isLoaded=isLoaded&(sscanf(buf,"%lf %s",r,dummy)>0);
	fgets(buf,1024,fp);
	isLoaded=isLoaded&(sscanf(buf,"%d %s",photonum,dummy)>0);
	fgets(buf,1024,fp);

	int t=1;
	while(1){
		sprintf(file,"nl%.2d_t%d_dt%d_r%d_o%d_t%d.dat",*nl,(int)(log10(*mt)),(int)(log10(*dt)),(int)((*r)*100),*photonum,t);
		FILE* fpt;
		fpt=fopen(file,"r");
		if(fpt==NULL){
			break;
		}else{
			fclose(fpt);
			t++;
		}
	}
	sprintf(potfile,"nl%.2d_t%d_dt%d_r%d_o%d_t%d.pdat",*nl,(int)(log10(*mt)),(int)(log10(*dt)),(int)((*r)*100+0.5),*photonum,t);

	if(!isLoaded){
		printf("parameter not found\n");return false;
	}
	return true;
}

int locate(int nl,double* x,double* y,char* ff){
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

			ff[n]=1;
			n++;

			if(n==2)break;
		}
	}
	return n;
}

void fluctuation(int n,double r,double* x,double* y,char* ff){
	int i;
	double theta;
	double len;
	for(i=0;i<n;i++){
		if(ff[i]==1){
			theta = 2*PI*(double)rand()/((double)RAND_MAX+1);
			len= r*(double)rand()/((double)RAND_MAX+1);
			x[i]+=len*cos(theta);
			y[i]+=len*sin(theta);
		}
	}
}

int main(int argc,char** argv){
	if(argc<2){
		printf("no command arg\n");return 0;
	}
	int nl;
	double dt;
	double mt;
	double r;
	int photonum;
	char file[1024];
	char potfile[1024];

	int n;
	long step;
	long maxstep;
	long outstep;
	double* x;
	double* y;
	double* fx;
	double* fy;
	double* fk1x;
	double* fk2x;
	double* fk3x;
	double* fk4x;
	double* fk1y;
	double* fk2y;
	double* fk3y;
	double* fk4y;
	double* fpotone;
	double fpotoneave;
	double fpotonedev;
	double fpotonemax;
	double fpotonemin;
	double* potone;
	double potoneave;
	double potonedev;
	double potonemax;
	double potonemin;
	char* ff;
	double t;
	double pot;
	double fpot;

	/* load */
	if(!load(argv[1],&nl,&mt,&dt,&r,&photonum,file,potfile)){
		printf("load fail\n");return 0;
	}
	printf("[Load]\nnl=%d\ndt=%.9lf\nmt=%lf\nrand=%lf\nphotonum=%d\nfile=%s\npotfile=%s\n",nl,dt,mt,r,photonum,file,potfile);

	/* setup */
	n = 3*nl*nl - 3*nl + 1;
	n=2;
	maxstep=(long)(mt/dt)+1;
	outstep=(maxstep/photonum)>0?(maxstep/photonum):1;
	x = new double[n];
	y = new double[n];
	fx = new double[n];
	fy = new double[n];
	ff = new char[n];
	fk1x = new double[n];
	fk2x = new double[n];
	fk3x = new double[n];
	fk4x = new double[n];
	fk1y = new double[n];
	fk2y = new double[n];
	fk3y = new double[n];
	fk4y = new double[n];
	fpotone = new double[n];
	potone = new double[n];
	srand((unsigned)time(NULL));

#ifdef _MSC_VER
test:
#endif

	/* locate */
	if(n!=locate(nl,x,y,ff)){
//		printf("incorrect location\n");return 0;
	}
	printf("[Initialize]\nn=%d\nstep=%ld\n",n,maxstep);

	/* fluctuation */
	fluctuation(n,r,x,y,ff);
	printf("[Fluctuation]\n");

	/* calc first potential */
	fpot=0;
	fpotonemax=0;
	for(int i=0;i<n;i++){
		fpotone[i]=0;
		for(int j=0;j<n;j++){
			if(i==j)continue;
			double dis6=pow(pow(x[i]-x[j],2)+pow(y[i]-y[j],2),-3);
			fpotone[i]+=4*ve*(sigma12*dis6-sigma6)*dis6;
			fpot+=4*ve*(sigma12*dis6-sigma6)*dis6;
		}
		if(i==0){
			fpotonemax=fpotone[i];
			fpotonemin=fpotone[i];
		}else{
			if(fpotonemax<fpotone[i]){
				fpotonemax=fpotone[i];
			}
			if(fpotonemin>fpotone[i]){
				fpotonemin=fpotone[i];
			}
		}
	}
	fpot/=2;
	fpotoneave=fpot/n*2;
	fpotonedev=0;
	for(int i=0;i<n;i++){
		fpotonedev += pow(fpotoneave-fpotone[i],2);
	}
	fpotonedev=sqrt(fpotonedev/(n-1));
	printf("[AnalyzeInitialPosition]\npotentialsum=%lf\npotentialave=%lf\npotentialdev=%lf\nfpotonemax=%lf\nfpotonemin=%lf\n"
		,fpot,fpotoneave,fpotonedev,fpotonemax,fpotonemin);


#ifdef _MSC_VER
rechoice:
	printf("ok?>");
	char keybuf[1024];
	fgets(keybuf,1024,stdin);
	if(keybuf[0]=='n')goto test;
	if(keybuf[0]!='y')goto rechoice;
#endif

	/* parallel */
#ifdef USE_OPENMP
	const int corenum=omp_get_num_procs();
	omp_set_num_threads(corenum);
	const int threadnum=omp_get_max_threads();
	printf("[Parallel]\ncorenum=%d\nthreadnum=%d\n",corenum,threadnum);
#else
	const int corenum=1;
	const int threadnum=1;
	printf("[Parallel]\nnot use parallel\n");
#endif

	/* main loop */
	printf("[MainLoopStart]\n");

	FILE* fp;

	fp = fopen(file,"w");
	if(fp==NULL){printf("cannot open output file\n");return 0;}
	fprintf(fp,"dt = %.9lf\n",dt);
	fprintf(fp,"n = %d\n",n);
	fprintf(fp,"# %ld\n",0);
	for(int i=0;i<n;i++){
		fprintf(fp,"%d %lf %lf\n",i,x[i],y[i]);
	}
	fclose(fp);

	fp = fopen(potfile,"w");
	if(fp==NULL){printf("cannot open potoutput file\n");return 0;}
	fprintf(fp,"%ld %lf %lf %lf %lf %lf %lf\n",0,0.0,fpot,fpotoneave,fpotonedev,fpotonemax,fpotonemin);
	fclose(fp);

	t=0;
	step=0;
	pot=0;

#ifdef USE_OPENMP
#pragma omp parallel
#endif
	{
#ifdef USE_OPENMP
		int myid = omp_get_thread_num();
#else
		int myid = 0;
#endif
		int mystart=n*myid/threadnum;
		int myend=n*(myid+1)/threadnum;
		int mi,mj;
		double mydis;
		double mydis6;
		double mylen;
		double mypot;
		double mypotdev;
		double mypotmax;
		double mypotmin;
		double mytemp;
		double mydifx;
		double mydify;

#ifdef USE_OPENMP
#pragma omp critical
#endif
		{
			printf("no%d %d - %d\n",myid,mystart,myend);
		}
#ifdef USE_OPENMP
#pragma omp barrier
#endif

		while(step<maxstep){

			/* calc potential */
			mypot=0;
			for(mi=mystart;mi<myend;mi++){
				potone[mi]=0;
				for(mj=0;mj<n;mj++){
					if(mi==mj)continue;
					mydis6=pow(pow(x[mi]-x[mj],2)+pow(y[mi]-y[mj],2),-3);
					mytemp=4*ve*(sigma12*mydis6-sigma6)*mydis6;
					mypot+=mytemp;
					potone[mi]+=mytemp;
				}

				if(mi==mystart){
					mypotmax=potone[mi];
					mypotmin=potone[mi];
				}else{
					if(mypotmax<potone[mi]){
						mypotmax=potone[mi];
					}
					if(mypotmin>potone[mi]){
						mypotmin=potone[mi];
					}
				}
			}
#ifdef USE_OPENMP
#pragma omp critical
#endif
			{
				pot+=mypot/2;
			}
#ifdef USE_OPENMP
#pragma omp barrier
#endif



			/* calc potential deviation */
			if(myid==0){
				potoneave=pot/n*2;
				potonemax=mypotmax;
				potonemin=mypotmin;
				potonedev=0;
			}
#ifdef USE_OPENMP
#pragma omp barrier
#endif
			mypotdev=0;
			for(mi=mystart;mi<myend;mi++){
				mypotdev+=pow(potoneave-potone[mi],2);
			}
#ifdef USE_OPENMP
#pragma omp critical
#endif
			{
				potonedev+=mypotdev;
				if(potonemax<mypotmax){
					potonemax=mypotmax;
				}
				if(potonemin>mypotmin){
					potonemin=mypotmin;
				}
			}
#ifdef USE_OPENMP
#pragma omp barrier
#endif
			if(myid==0){
				potonedev=sqrt(potonedev/(n-1));
			}
#ifdef USE_OPENMP
#pragma omp barrier
#endif



			/* calc force */

			/*
			fx;
			fy;
			fk1x;
			fk2x;
			fk3x;
			fk4x;
			fk1y;
			fk2y;
			fk3y;
			fk4y;
			*/

			/* calc k1 */
			for(mi=mystart;mi<myend;mi++){
				fx[mi]=0;
				fy[mi]=0;
				for(mj=0;mj<n;mj++){
					if(mi==mj)continue;
					mydifx=x[mi]-x[mj];
					mydify=y[mi]-y[mj];
					mydis=hypot(mydifx,mydify);
					if(mydis<eps)mydis=eps;
					mydis6=pow(mydis,-6);
					mylen=24*ve*( 2*sigma12*mydis6 - sigma6 )*mydis6/mydis;
					fx[mi]+=-mylen*mydifx/mydis;
					fy[mi]+=-mylen*mydify/mydis;
				}
			}
#ifdef USE_OPENMP
#pragma omp barrier
#endif
			for(mi=mystart;mi<myend;mi++){
				fk1x[mi]=fy[mi];
				fk1y[mi]=-fx[mi];
			}
#ifdef USE_OPENMP
#pragma omp barrier
#endif

			/* calc k2 */
			for(mi=mystart;mi<myend;mi++){
				fx[mi]=0;
				fy[mi]=0;
				for(mj=0;mj<n;mj++){
					if(mi==mj)continue;
					mydifx=(x[mi]+fk1x[mi]*dt/2)-(x[mj]+fk1x[mj]*dt/2);
					mydify=(y[mi]+fk1y[mi]*dt/2)-(y[mj]+fk1y[mj]*dt/2);
					mydis=hypot(mydifx,mydify);
					if(mydis<eps)mydis=eps;
					mydis6=pow(mydis,-6);
					mylen=24*ve*( 2*sigma12*mydis6 - sigma6 )*mydis6/mydis;
					fx[mi]+=-mylen*mydifx/mydis;
					fy[mi]+=-mylen*mydify/mydis;
				}
			}
#ifdef USE_OPENMP
#pragma omp barrier
#endif
			for(mi=mystart;mi<myend;mi++){
				fk2x[mi]=fy[mi]*dt;
				fk2y[mi]=-fx[mi]*dt;
			}
#ifdef USE_OPENMP
#pragma omp barrier
#endif

			/* calc k3 */
			for(mi=mystart;mi<myend;mi++){
				fx[mi]=0;
				fy[mi]=0;
				for(mj=0;mj<n;mj++){
					if(mi==mj)continue;
					mydifx=(x[mi]+fk2x[mi]*dt/2)-(x[mj]+fk2x[mj]*dt/2);
					mydify=(y[mi]+fk2y[mi]*dt/2)-(y[mj]+fk2y[mj]*dt/2);
					mydis=hypot(mydifx,mydify);
					if(mydis<eps)mydis=eps;
					mydis6=pow(mydis,-6);
					mylen=24*ve*( 2*sigma12*mydis6 - sigma6 )*mydis6/mydis;
					fx[mi]+=-mylen*mydifx/mydis;
					fy[mi]+=-mylen*mydify/mydis;
				}
			}
#ifdef USE_OPENMP
#pragma omp barrier
#endif
			for(mi=mystart;mi<myend;mi++){
				fk3x[mi]=fy[mi]*dt;
				fk3y[mi]=-fx[mi]*dt;
			}
#ifdef USE_OPENMP
#pragma omp barrier
#endif

			/* calc k4 */
			for(mi=mystart;mi<myend;mi++){
				fx[mi]=0;
				fy[mi]=0;
				for(mj=0;mj<n;mj++){
					if(mi==mj)continue;
					mydifx=(x[mi]+fk3x[mi]*dt)-(x[mj]+fk3x[mj]*dt);
					mydify=(y[mi]+fk3y[mi]*dt)-(y[mj]+fk3y[mj]*dt);
					mydis=hypot(mydifx,mydify);
					if(mydis<eps)mydis=eps;
					mydis6=pow(mydis,-6);
					mylen=24*ve*( 2*sigma12*mydis6 - sigma6 )*mydis6/mydis;
					fx[mi]+=-mylen*mydifx/mydis;
					fy[mi]+=-mylen*mydify/mydis;
				}
			}
#ifdef USE_OPENMP
#pragma omp barrier
#endif
			for(mi=mystart;mi<myend;mi++){
				fk4x[mi]=fy[mi]*dt;
				fk4y[mi]=-fx[mi]*dt;
			}
#ifdef USE_OPENMP
#pragma omp barrier
#endif
			/* calc step */
			for(mi=mystart;mi<myend;mi++){
				x[mi]+=(dt/6)*(fk1x[mi]+2*fk2x[mi]+2*fk3x[mi]+fk4x[mi]);
				y[mi]+=(dt/6)*(fk1y[mi]+2*fk2x[mi]+2*fk3x[mi]+fk4x[mi]);

				double mmytheta = 2*PI*(double)rand()/((double)RAND_MAX+1);
				double mmylen= randdiv*(double)rand()/((double)RAND_MAX+1);

				x[mi]+=mmylen*cos(mmytheta)*dt;
				y[mi]+=mmylen*sin(mmytheta)*dt;
			}
#ifdef USE_OPENMP
#pragma omp barrier
#endif

			/* step time , output*/
			if(myid==0){
				step++;

				if(step%outstep==0){
					printf("# %ld\n",step);

					fp=fopen(file,"a");
					fprintf(fp,"# %ld\n",step);
					for(mi=0;mi<n;mi++){
						fprintf(fp,"%d %lf %lf\n",mi,x[mi],y[mi]);
					}
					fclose(fp);

					fp=fopen(potfile,"a");
					fprintf(fp,"%ld %lf %lf %lf %lf %lf %lf\n",step,t,pot,potoneave,potonedev,potonemax,potonemin);
					fclose(fp);
				}

				t+=dt;
				pot=0;

			}
#ifdef USE_OPENMP
#pragma omp barrier
#endif
		}
	}
	printf("[MainLoopEnd]\n");

	/* close */
/*	delete []x;
	delete []y;
	delete []fx;
	delete []fy;
	delete []ff;
	delete []fpotone;
	delete []potone;*/
	printf("[Close]\n");
	return 0;
}