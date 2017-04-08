

#include "SkyrmionSimulator.h"
#include "ApplicationProperty.h"

//#define OPENMP_SINGLE_LOOP
#define OPENMP_DOUBLE_LOOP

int SkyrmionSimulator::Init(){

	InitParameter();
	InitPosition();

	if(n>=MAXN){
		cout << "too many particles" << endl;
		return -1;
	}

	return 0;
}

void SkyrmionSimulator::InitParameter(){
	t=0;
	psum=0;
	sr=(signed)time(NULL);

	ballGraphicDivide=AP::GetInt("Graphic","BallDivide",8);

	outputFile=AP::GetString("Output","OutputFile","Output.txt");
	maxFrame=AP::GetInt("Output","MaxFrame",1000000);
	outputResultFlag=(AP::GetInt("Output","SaveResults",1)==1);
	outputResultInterval=AP::GetInt("Output","SaveFrameInterval",100);
	consolePrintResultInterval=AP::GetInt("Output","PrintFrameInterval",1000);

	dt=AP::GetDouble("Setup","DeltaTime",1.0e-4);
	prand=AP::GetDouble("Setup","PositionRandomness",0.3);

	cr=AP::GetDouble("Graphic","ParticleColorRed",1.0);
	cg=AP::GetDouble("Graphic","ParticleColorGreen",1.0);
	cb=AP::GetDouble("Graphic","ParticleColorBlue",1.0);

	ve=AP::GetDouble("Coefficient","PotentialDepth",1.0e-0);
	sigma=AP::GetDouble("Coefficient","PotentialSigmaD",1.0e-0)*pow(2,-1.0/6);

	cpotential=AP::GetDouble("Coefficient","CirclePotentialDepth",1.0e-0);

	pfv=AP::GetDouble("Coefficient","RateF-V",1.0e0);	

	borderShape = AP::GetInt("Location","BorderShape",0); // 0 square , 1 circle
	borderPositionInterval = AP::GetDouble("Location","BorderPositionInterval",1e-0);

	squareBorderPositionType=AP::GetInt("LocationSquare","SquareBorderPositionType",0); //0 square , 1 tri
	squareBorderLoopLR	=AP::GetInt("LocationSquare","SquareBorderLoopLR",0);
	squareBorderLoopUD	=AP::GetInt("LocationSquare","SquareBorderLoopUD",0);
	squareBorderSolidLR	=AP::GetInt("LocationSquare","SquareBorderSolidLR",0);
	squareBorderSolidUD	=AP::GetInt("LocationSquare","SquareBorderSolidUD",0);

	squareBorderPositionRowNumber = AP::GetInt("LocationSquare","SquareBorderPositionRowNumber",5);
	squareBorderPositionColNumber = AP::GetInt("LocationSquare","SquareBorderPositionColNumber",5);
	circleBorderPositionType = AP::GetInt("LocationCircle","CircleBorderPositionType",0);//0 square , 1 tri , 2 circle
	circleBorderSolid		=AP::GetInt("LocationCircle","CircleBorderSolid",0);
	circleBorderPositionRadiusNumber = AP::GetInt("LocationCircle","CircleBorderPositionRadiusNumber",3);

	sigma12 = pow(sigma,12);
	sigma6 = pow(sigma,6);

	dif[0][0]=0;	dif[0][1]=0;
	dif[1][0]=1;	dif[1][1]=0;
	dif[2][0]=-1;	dif[2][1]=0;
	dif[3][0]=0;	dif[3][1]=1;
	dif[4][0]=0;	dif[4][1]=-1;
	dif[5][0]=1;	dif[5][1]=1;
	dif[6][0]=-1;	dif[6][1]=1;
	dif[7][0]=1;	dif[7][1]=-1;
	dif[8][0]=-1;	dif[8][1]=-1;

	borderStart=1;
	borderEnd=-1;
	if(borderShape==0){
		//境界は□
		if(squareBorderLoopLR && squareBorderLoopUD){
			borderStart=1;
			borderEnd=8;
		}
		if(squareBorderLoopLR && squareBorderLoopUD==0){
			borderStart=1;
			borderEnd=2;
		}
		if(squareBorderLoopLR==0 && squareBorderLoopUD){
			borderStart=3;
			borderEnd=4;
		}
	}
}

void SkyrmionSimulator::putCircle(pdVertex2d r,double rad){

	double theta=((rand()%360)/180.0*PI);
	double leng=rad*prand*(rand()%1000)/1000.0;

	if(rand()%6==0){
		x[n]=r.x + cos(theta)*leng;
		y[n]=r.y + sin(theta)*leng;
	}else{
		x[n]=r.x;
		y[n]=r.y;
	}
	vx[n]=0;
	vy[n]=0;
	vbx[n]=0;
	vby[n]=0;
	n++;
}

void SkyrmionSimulator::InitPosition(){
	n=0;
	double interval = borderPositionInterval;

	int x,y;
	if(borderShape==0){
		//境界は□
		if(squareBorderPositionType==0){
			//配置が□
			w=interval*squareBorderPositionColNumber;
			h=interval*squareBorderPositionRowNumber;
			for(y=0;y<squareBorderPositionRowNumber;y++){
				for(x=0;x<squareBorderPositionColNumber;x++){
					pdVertex2d r=pdVertex2d(
						-w/2+interval/2		+interval*x
						,-h/2+interval/2	+interval*y
						);
					putCircle(r,interval/2);
				}
			}
		}else{
			//配置が△
			double hinterval = interval*sqrt(3.0)/2.0;
			w=interval*squareBorderPositionColNumber;
			h=hinterval*(squareBorderPositionRowNumber-1)+interval;
			for(y=0;y<squareBorderPositionRowNumber;y++){
				for(x=0;x<squareBorderPositionColNumber;x++){

					pdVertex2d r;
					if(y%2==0){
						r=pdVertex2d(
							-w/2+interval/2		+interval*x
							,-h/2+hinterval/2	+hinterval*y
							);
					}else{
						r=pdVertex2d(
							-w/2+interval/2		+interval*x - interval/2
							,-h/2+hinterval/2	+hinterval*y
							);
					}
					putCircle(r,interval/2);
				}
			}
		}
	}else{
		//境界は○
		br=interval*circleBorderPositionRadiusNumber-interval/2;
		if(circleBorderPositionType==0){
			//配置が□
			for(y=0;y<circleBorderPositionRadiusNumber*sqrt(3.03)/2+3;y++){
				for(x=0;x<circleBorderPositionRadiusNumber;x++){
					pdVertex2d r;
					r=pdVertex2d(
						interval*x
						,interval*y
						);
					if(br-r.length()>=interval/2){
						putCircle(r,interval/2);
						if(x!=0){
							r.x*=(-1);
							putCircle(r,interval/2);
							r.x*=(-1);
						}
						if(y!=0){
							r.y*=(-1);
							putCircle(r,interval/2);
							r.y*=(-1);
						}
						if(x!=0 && y!=0){
							r.x*=(-1);
							r.y*=(-1);
							putCircle(r,interval/2);
							r.x*=(-1);
							r.y*=(-1);
						}
					}
				}
			}
		}else if(circleBorderPositionType==1){
			//配置が△
			double hinterval = interval*sqrt(3.0)/2.0;
			for(y=0;y<circleBorderPositionRadiusNumber*2/sqrt(3.0)+10;y++){
				for(x=0;x<circleBorderPositionRadiusNumber;x++){
					pdVertex2d r;
					if(y%2==0){
						r=pdVertex2d(
							interval*x
							,hinterval*y
							);
					}else{
						r=pdVertex2d(
							interval*x + interval/2
							,hinterval*y
							);
					}

					if(br-r.length()>=interval/2)	putCircle(r,interval/2);
					if(x!=0 || y%2!=0){
						r.x*=(-1);
						if(br-r.length()>=interval/2)	putCircle(r,interval/2);
						r.x*=(-1);
					}
					if(y!=0){
						r.y*=(-1);
						if(br-r.length()>=interval/2)	putCircle(r,interval/2);
						r.y*=(-1);
					}
					if((x!=0 || y%2!=0) && y!=0){
						r.x*=(-1);
						r.y*=(-1);
						if(br-r.length()>=interval/2)	putCircle(r,interval/2);
						r.x*=(-1);
						r.y*=(-1);
					}
				}
			}
		}else{
			//配置が○
			pdVertex2d r;
			r=pdVertex2d(0,0);
			putCircle(r,interval/2);
			for(int rn=1;rn<circleBorderPositionRadiusNumber;rn++){
				double rtc=acos(1.0-(1.0/(2.0*rn*rn)));
				for(double rt=0;rt<2*PI-rtc;rt+=rtc){
					r=pdVertex2d(
						rn*interval*cos(rt),
						rn*interval*sin(rt)
						);
					if(br-r.length()>=interval/2){
						putCircle(r,interval/2);
					}
				}
			}
		}
	}
}
/*
pdVertex2d SkyrmionSimulator::CalcInteractionForce(pdVertex2d r1,pdVertex2d r2){
	pdVertex2d rij = r1-r2;
	double dis=rij.length();
	double dis7=pow(dis,-7);
	double dis13=dis7*dis7*dis;
	if(dis<EPS)dis=EPS;
	psum+=4*ve*(sigma12*pow(dis,-12)-sigma6*pow(dis,-6))+psums;
	return (rij/dis)*24*ve*(2*sigma12*dis13-sigma6*dis7);
}
*/
void SkyrmionSimulator::CalcInteractionForce(double x1,double y1,double x2,double y2,double* px,double* py){
	double dis=hypot(x1-x2,y1-y2);
	if(dis<EPS)dis=EPS;	
	double dis6=pow(dis,-6);
	psum+=4*ve*(sigma12*dis6-sigma6)*dis6;
	double len=24*ve*( 2*sigma12*dis6 - sigma6 )*dis6/dis;
	(*px)=len*(x1-x2)/dis;
	(*py)=len*(y1-y2)/dis;
}

/*
void SkyrmionSimulator::CalcForceField(){
	int i;
#ifdef OPENMP_SINGLE_LOOP
#ifdef _OPENMP
#pragma omp parallel for
#endif
#endif
	for(i=0;i<n;i++){
		if(r[i].length()<EPS)f[i]+=r[i]*cpotential/EPS;
		else f[i]+=r[i]*cpotential/r[i].length();
		psum+=cpotential*r[i].length()*r[i].length()/2.0+psums;
	}
}
*/


void SkyrmionSimulator::CalcForceCollision(){
	int i,j,k;
	double px,py;
#ifdef OPENMP_DOUBLE_LOOP
#ifdef _OPENMP
#pragma omp parallel for private(j,k,px,py)
#endif
#endif
	for(i=0;i<n;i++){
		for(j=i;j<n;j++){
			k=0;
			if(j!=i){
				CalcInteractionForce(x[i],y[i],x[j],y[j],&px,&py);
				fx[i]+=px;
				fy[i]+=py;
				fx[j]-=px;
				fy[j]-=py;
			}
			for(k=borderStart;k<=borderEnd;k++){
				CalcInteractionForce(x[i],y[i],x[j]+w*dif[k][0],y[j]+h*dif[k][1],&px,&py);
				fx[i]+=px;
				fy[j]-=py;
			}
		}
	}
}

void SkyrmionSimulator::CalcForceWall(){
	int i;
	double px,py;
	if(borderShape==0){
		//境界は□
		if(squareBorderSolidLR && squareBorderSolidUD==0){
#ifdef OPENMP_SINGLE_LOOP
#ifdef _OPENMP
#pragma omp parallel for private(px,py)
#endif
#endif
			for(i=0;i<n;i++){
				CalcInteractionForce(x[i],y[i],-w/2-borderPositionInterval/2,y[i],&px,&py);
				fx[i]+=px;
				fy[i]+=py;
				CalcInteractionForce(x[i],y[i],w/2+borderPositionInterval/2,y[i],&px,&py);
				fx[i]+=px;
				fy[i]+=py;
			}

		}
		if(squareBorderSolidLR==0 && squareBorderSolidUD){
#ifdef OPENMP_SINGLE_LOOP
#ifdef _OPENMP
#pragma omp parallel for
#endif
#endif
			for(i=0;i<n;i++){
				CalcInteractionForce(x[i],y[i],x[i],-h/2-borderPositionInterval/2,&px,&py);
				fx[i]+=px;
				fy[i]+=py;
				CalcInteractionForce(x[i],y[i],x[i], h/2+borderPositionInterval/2,&px,&py);
				fx[i]+=px;
				fy[i]+=py;
			}
		}
		if(squareBorderSolidLR && squareBorderSolidUD){
#ifdef OPENMP_SINGLE_LOOP
#ifdef _OPENMP
#pragma omp parallel for
#endif
#endif
			for(i=0;i<n;i++){
				CalcInteractionForce(x[i],y[i],x[i],-h/2-borderPositionInterval/2,&px,&py);
				fx[i]+=px;fy[i]+=py;
				CalcInteractionForce(x[i],y[i],x[i], h/2+borderPositionInterval/2,&px,&py);
				fx[i]+=px;fy[i]+=py;
				CalcInteractionForce(x[i],y[i],-w/2-borderPositionInterval/2,y[i],&px,&py);
				fx[i]+=px;fy[i]+=py;
				CalcInteractionForce(x[i],y[i],w/2+borderPositionInterval/2,y[i],&px,&py);
				fx[i]+=px;fy[i]+=py;
			}
		}
	}else{
		//境界は○
		if(circleBorderSolid){
#ifdef OPENMP_SINGLE_LOOP
#ifdef _OPENMP
#pragma omp parallel for
#endif
#endif
			for(i=0;i<n;i++){
				double dis=hypot(x[i],y[i]);
				if(dis<borderPositionInterval/2)continue;
				double wp=(br+borderPositionInterval/2)/dis;
				CalcInteractionForce(x[i],y[i],x[i]*wp,y[i],&px,&py);
				fx[i]+=px;
				fy[i]+=py;
			}
		}
	}
}

void SkyrmionSimulator::CalcForce(){
	int i;
#ifdef OPENMP_SINGLE_LOOP
#ifdef _OPENMP
#pragma omp parallel for
#endif
#endif
	for(i=0;i<n;i++){
		fx[i]=0;
		fy[i]=0;
	}

	CalcForceCollision();

	CalcForceWall();

//	CalcForceField();
}

void SkyrmionSimulator::CalcVectorSkyrmion(){
	int i;
//	if(scaling)psumratio=psumf/psum;
//	else psumratio=1.0;

#ifdef OPENMP_SINGLE_LOOP
#ifdef _OPENMP
#pragma omp parallel for
#endif
#endif
	for(i=0;i<n;i++){
		vx[i]=fy[i]*pfv;
		vy[i]=-fx[i]*pfv;
		x[i]+=vx[i]*dt + (vx[i]-vbx[i])*dt*dt/2.0;
		y[i]+=vy[i]*dt + (vy[i]-vby[i])*dt*dt/2.0;
		vbx[i]=vx[i];
		vby[i]=vy[i];
	}

}
/*
void SkyrmionSimulator::SetParam(){
	int i;

#ifdef OPENMP_SINGLE_LOOP
#ifdef _OPENMP
#pragma omp parallel for
#endif
#endif
	for(i=0;i<n;i++){
		pdParam2d par=GetBody(i)->GetParam2d();
		par.r=r[i];
		par.v=v[i];
		GetBody(i)->SetParam(par);
	}

}
*/
int SkyrmionSimulator::Step(void){
	t+=dt;
	psum=0;

	CalcForce();

	if(frameCount==0){
		psumf=psum;
	}

	CalcVectorSkyrmion();

	if(borderShape==0){
		if(squareBorderLoopLR){
			for(int i=0;i<n;i++){
				if(x[i]>w/2)x[i]-=w;
				if(x[i]<-w/2)x[i]+=w;
			}
		}
		if(squareBorderLoopUD){
			for(int i=0;i<n;i++){
				if(y[i]>h/2)y[i]-=h;
				if(y[i]<-h/2)y[i]+=h;
			}
		}
	}
	frameCount++;
	if(frameCount>maxFrame)return -1;
	return 0;
}

#ifdef _MSC_VER
int SkyrmionSimulator::Draw(void){

	double theta,d;
	for(int i=0;i<n;i++){
		d=PI/ballGraphicDivide;
		glPushMatrix();
		glBegin(GL_LINE_STRIP);
		for (theta=0;theta<2*PI+d;theta+=d)
		{
			glColor4d(cr,cg,cb,1.0);
			glVertex2d(x[i]+borderPositionInterval/8*cos(theta) , y[i]+borderPositionInterval/8*sin(theta) );
		}
		glEnd();						 
		glPopMatrix();
	}

	if(borderShape==0){
		pdVertex2d a[5];
		pdColor c[5];
		a[0]=a[4]=pdVertex2d(	-w/2	,-h/2);
		a[1]=pdVertex2d(		w/2		,-h/2);
		a[2]=pdVertex2d(		w/2		,h/2);
		a[3]=pdVertex2d(		-w/2	,h/2);
		c[0]=c[1]=c[2]=c[3]=c[4]=pdColor(1,1,1);
		pdDrawLine(a,c,5);
	}else{
		pdDrawCircle(pdVertex2d(0,0),pdColor(1,1,1),br);
	}

	char str[256];
	sprintf(str,"Time %.4lf",t);
	pdDrawString(str,pdVertex2d(0,0),pdColor(1,1,1));
	sprintf(str,"P0 %.4lf",psumf);
	pdDrawString(str,pdVertex2d(0,20),pdColor(1,1,1));
	sprintf(str,"P  %.4lf",psum);
	pdDrawString(str,pdVertex2d(0,40),pdColor(1,1,1));
	return 0;
}
#endif

double SkyrmionSimulator::drand(){
	sr=sr*65539;
	if(sr<0)sr=(sr+2147483647)+1;
	return sr*0.4656613e-9;
}

int SkyrmionSimulator::WriteData(){
	if(outputResultFlag==0)return 0;
	int i;
	FILE* fp;
	FILE* fpp;
	if(GetFrameCount()%consolePrintResultInterval==0){
		cout << "# Frame : " << GetFrameCount() << endl;
	}
	if(GetFrameCount()%outputResultInterval!=0)return 0;

	if(GetFrameCount()==0){
		fp=fopen(outputFile.c_str(),"w");
		if(fp==NULL){
			cout << "File Open Error" << endl;
			return 0;
		}
		fprintf(fp,"dt = %lf\n",dt);
		fprintf(fp,"n = %d\n",n);

		string outputFilePot=outputFile+".p.txt";
		fpp=fopen(outputFilePot.c_str(),"w");
		if(fpp==NULL){
			cout << "File Open Error" << endl;
			return 0;
		}

	}else{

		fp=fopen(outputFile.c_str(),"a");
		if(fp==NULL){
			return 0;
		}


		string outputFilePot=outputFile+".p.txt";
		fpp=fopen(outputFilePot.c_str(),"a");
		if(fpp==NULL){
			return 0;
		}

	}

	fprintf(fp,"# %ld\n",GetFrameCount());
	for(i=0;i<n;i++){
		fprintf(fp,"%d %lf %lf %lf %lf\n",i,x[i],y[i],0.0,borderPositionInterval);
	}

	if(GetFrameCount()!=0){
		if(GetFrameCount()==outputResultInterval){
			fprintf(fpp,"0 %lf\n",psumf);
		}
		fprintf(fpp,"%lf %lf\n",GetFrameCount()*dt,psum);
	}

	fclose(fp);
	fclose(fpp);
	return 0;
}
