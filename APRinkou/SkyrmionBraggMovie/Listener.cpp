
#include "Listener.h"
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
#pragma comment(lib,"cv210")
#pragma comment(lib,"cxcore210")
#pragma comment(lib,"highgui210")
#pragma comment(lib,"cvaux210")
#endif

#define PI (2.0*acos(0.0))

bool Listener::Init(int argc,char** argv){

	bool isCorrect=true;
	drawCount=0;

	frc=0;
	frl.clear();
	x.clear();
	y.clear();
	if(argc<=1){printf("no argument\n");return false;}

	FILE *fp;
	char buf[1024];
	string str=argv[1];
	sprintf(outfile,"%s",str.c_str());
	sprintf(outvfile,"%s_bragg.avi",str.substr(0,str.find_first_of(".")).c_str());

	char cdir[1024];
	GetCurrentDirectory(1024,cdir);

	printf("[Setup]\ncurrentdir=%s\noutfile=%s\noutvfile=%s\n",cdir,outfile,outvfile);

	fp=fopen(outfile,"r");
	if(fp==NULL){printf("out file not found\n");return false;}

	fgets(buf,1024,fp);
	isCorrect=isCorrect&(sscanf_s(buf,"dt = %lf",&dt)>0);
	if(dt==0){
		printf("dt is too small dt=1e-8 instead\n");
		dt=1e-8;
	}
	fgets(buf,1024,fp);
	isCorrect=isCorrect&(sscanf_s(buf,"n = %d",&n)>0);

	printf("[LoadStart]\n");
	int dfr;
	int di;
	double px,py;
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
			tx[i]=px;
			ty[i]=py;
		}
		if(isCorrect){
			x.push_back(tx);
			y.push_back(ty);
			frl.push_back(dfr);
		}

	}
	fclose(fp);

	printf("[LoadEnd]\nframelength=%d\ntimelength=%lf\n",frl.size(),dt*frl[frl.size()-1]);
	isVideo=false;
	isCheck=true;
	isDraw=false;
	isEnd=false;
	video=NULL;
	if(!isCorrect){printf("invalid parameter\n");return false;}

	qt=(MAXLEN/SIZE);
	CalcCurrent();

	return true;
}

void Listener::CalcCurrent(){
	int i,j,k;
	double qx,qy;
	maxNorm=0;
	for(i=-SIZE;i<=SIZE;i++){
		for(j=-SIZE;j<=SIZE;j++){
			qx=j*qt;
			qy=i*qt;
			cval[i+SIZE][j+SIZE]=complex<double>(0,0);
			for(k=0;k<n;k++){
				double cx=x[frc][k];
				double cy=y[frc][k];
				double cr=hypot(cx,cy);
				cval[i+SIZE][j+SIZE]+=complex<double>(cos(qx*cx+qy*cy),sin(qx*cx+qy*cy))*exp(-cr*cr/(2*qt))*PI/qt;
			}
			cnorm[i+SIZE][j+SIZE]=norm(cval[i+SIZE][j+SIZE]);
			if(maxNorm<cnorm[i+SIZE][j+SIZE]){
				maxNorm=cnorm[i+SIZE][j+SIZE];
			}
		}
	}
	for(i=-SIZE;i<=SIZE;i++){
		for(j=-SIZE;j<=SIZE;j++){
			cnorm[i+SIZE][j+SIZE]/=maxNorm;
		}
	}
}

int Listener::Timer(){
	if(isCheck){
		glutPostRedisplay();
		if(isEnd){
			glutLeaveMainLoop();
		}
		return (1000/60);
	}else{
		if(isDraw){
			if(isVideo && video!=NULL){
				if(drawCount<5)drawCount++;
				else{
					glReadPixels(0,0,glutGet(GLUT_WINDOW_WIDTH),glutGet(GLUT_WINDOW_HEIGHT),GL_RGB,GL_UNSIGNED_BYTE,data);
					memcpy(img->imageData,data,img->imageSize);
					cvCvtColor(img,img,CV_BGR2RGB);
					cvFlip(img,NULL,0);
					cvWriteFrame(video,img);
					drawCount=0;
				}
			}

			frc++;
			if(isVideo){
				if(frc>=frl.size() || isEnd){
					if(video!=NULL){
						cvReleaseVideoWriter(&video);
						cvReleaseImage(&img);
						delete []data;
					}
					glutLeaveMainLoop();
				}else{
					CalcCurrent();
				}
			}else{
				if(isEnd){
					glutLeaveMainLoop();
				}
				if(frc>=frl.size()){
					frc=frl.size();
					isCheck=true;
				}else{
					CalcCurrent();
				}
			}

			isDraw=false;
			glutPostRedisplay();
		}else{
			glutPostRedisplay();
		}

		if(isVideo){
			return 0;
		}else{
			return sp;
		}
	}
}

void Listener::Disp(){

	unsigned int cfrc=frc;
	bool isReach=false;
	if(cfrc>=frl.size()){
		cfrc=frl.size()-1;
		isReach=true;
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_BLEND);
	glLoadIdentity();
	gluLookAt(camx, camy, camz , desx, desy, desz, 0.0, 1.0, 0.0);


	glPushMatrix();
	for(int i=0;i<SIZE*2;i++){
		glBegin(GL_TRIANGLE_STRIP);
		for(int j=0;j<SIZE*2+1;j++){
			glColor4d(1.0,1.0,1.0,cnorm[i][j]);
			glVertex2d( (j-SIZE)*qt , (i-SIZE)*qt );

			glColor4d(1.0,1.0,1.0,cnorm[i+1][j]);
			glVertex2d( (j-SIZE)*qt , (i+1-SIZE)*qt );
		}
		glEnd();
	}
	glPopMatrix();

	glColor4d(1.0,1.0,1.0,1.0);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();

	glLoadIdentity();
	gluOrtho2D(0,glutGet(GLUT_WINDOW_WIDTH), 0, glutGet(GLUT_WINDOW_HEIGHT));
	glPushAttrib(GL_CURRENT_BIT|GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);

	char str[1024];

	sprintf(str,"Time %.8lf",dt*frl[cfrc]);
	glRasterPos2d(0,0); //左下が原点
	for(unsigned int i=0;i<strlen(str);i++){
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,str[i]);
	}

	sprintf(str,"Frame %d",frl[cfrc]);
	glRasterPos2d(0,20); //左下が原点
	for(unsigned int i=0;i<strlen(str);i++){
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,str[i]);
	}

	if(isCheck){
		sprintf(str,"Video %s",isVideo?"ON":"OFF");
		glRasterPos2d(0,40); //左下が原点
		for(unsigned int i=0;i<strlen(str);i++){
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,str[i]);
		}

		sprintf(str,"Speed %d ms/draw",sp);
		glRasterPos2d(0,60); //左下が原点
		for(unsigned int i=0;i<strlen(str);i++){
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,str[i]);
		}

		if(isReach){
			sprintf(str,"End");
			glRasterPos2d(0,80); //左下が原点
			for(unsigned int i=0;i<strlen(str);i++){
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,str[i]);
			}
		}
	}

	glPopAttrib();			
	glPopMatrix(); 
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();//もとの状態にもどる


	glDisable(GL_BLEND);
	glutSwapBuffers();
	isDraw=true;
}

void Listener::Keyboard(unsigned char key){
	switch(key){
	case 'r':
		if(!isVideo){
			frc=0;
		}
		break;
	case '-':
		sp-=dsp;
		if(sp<0)sp=0;
		break;
	case '+':
		sp+=dsp;
		break;

	case VK_RETURN:
		if(isCheck){
			isCheck=false;
			if(isVideo && video==NULL){
				video=cvCreateVideoWriter(outvfile,0,60,cvSize(glutGet(GLUT_WINDOW_WIDTH),glutGet(GLUT_WINDOW_HEIGHT)));
				data= new char[glutGet(GLUT_WINDOW_WIDTH)*glutGet(GLUT_WINDOW_HEIGHT)*3];
				img=cvCreateImage(cvSize(glutGet(GLUT_WINDOW_WIDTH),glutGet(GLUT_WINDOW_HEIGHT)),IPL_DEPTH_8U,3);
			}
		}else{
			isCheck=true;
		}
		break;

	case VK_ESCAPE:
		isEnd=true;
		break;
	}

	if(!isVideo){
		switch(key){
		case 'w':
			camy+=kdxy;
			desy+=kdxy;
			break;
		case 's':
			camy-=kdxy;
			desy-=kdxy;
			break;
		case 'a':
			camx-=kdxy;
			desx-=kdxy;
			break;
		case 'd':
			camx+=kdxy;
			desx+=kdxy;
			break;

		case 'z':
			camz-=kdz;
			break;
		case 'x':
			camz+=kdz;
			break;
		}
	}

	if(isCheck){
		switch(key){
		case 'v':
			isVideo=!isVideo;
			break;
		}
	}
}