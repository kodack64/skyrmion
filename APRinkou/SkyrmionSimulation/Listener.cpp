
#include "Common.h"
#include "Listener.h"
#include "ApplicationProperty.h"

#ifdef _MSC_VER

Listener* listener = new Listener;

int Listener::Init(Simulator* _sim){
	sim=_sim;
	windowSizeX=AP::GetInt("Graphic","WindowSizeX",640);
	windowSizeY=AP::GetInt("Graphic","WindowSizeY",480);
	windowPosX=AP::GetInt("Graphic","WindowPosX",100);
	windowPosY=AP::GetInt("Graphic","WindowPosY",100);
	fullscreen=(AP::GetInt("Graphic","Fullscreen",0)==1);
	stepFps=AP::GetDouble("Graphic","StepPerSecond",60);
	drawFps=AP::GetDouble("Graphic","DrawPerSecond",1.0);

	//make window
	glutInit(&(AP::argc),AP::argv);
	glutInitWindowSize(windowSizeX,windowSizeY);
	glutInitWindowPosition(windowPosX,windowPosY);
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE,GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH);
	glutCreateWindow("Skyrmion Simulation");

	//event regist
	glutTimerFunc(0,(Listener::_Timer),0);
	glutDisplayFunc(Listener::_Disp);
	glutMouseFunc(Listener::_Mouse);
	glutReshapeFunc(Listener::_Reshape);
	glutKeyboardFunc(Listener::_Keyboard);

	//display init
	glClearColor(0.0, 0.0, 0.0, 1.0);
	if(fullscreen!=0)glutFullScreen();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	lastElapsedTime=glutGet(GLUT_ELAPSED_TIME);
	graphicCallCount=0;
	elapsedFrame=0;
	enableGraphic=true;

	//set camera
	cam.o.x=cam.eye.x=AP::GetDouble("Graphic","CameraX",0.0);
	cam.o.y=cam.eye.y=AP::GetDouble("Graphic","CameraY",0.0);
	cam.o.z=0.0;
	cam.eye.z=AP::GetDouble("Graphic","CameraZ",6.0);
	cam.VertexToPolar();
	return 0;
}

int Listener::Timer(){

	assert(sim!=0);
	
	sim->WriteData();
	
	if(sim->Step()==-1){
		printf("End\n");
		glutLeaveMainLoop();
	}

	int elpget=glutGet(GLUT_ELAPSED_TIME);
	int elp =elpget-lastElapsedTime;

	if(elp>=1000){
		char title[256];
		sprintf(title,"Particle Simulation FPS:%d/%d",elapsedFrame,stepFps);
		if(!enableGraphic){
			sprintf(title,"%s No Graphic",title);
		}
		glutSetWindowTitle(title);
		elapsedFrame=0;
		graphicCallCount=0;
		lastElapsedTime=glutGet(GLUT_ELAPSED_TIME);
		elp=0;
	}
	elapsedFrame++;

	if(drawFps<stepFps){
		if(drawFps>0){
			glutPostRedisplay();
		}
	}else{
		if(1000*graphicCallCount/drawFps<elp){
			graphicCallCount++;
			glutPostRedisplay();
		}
	}

	if(stepFps>0){
		int diff=(int)(1000*elapsedFrame/stepFps-elp);
		if(diff>0)glutTimerFunc(diff,_Timer,0);
		else glutTimerFunc(0,_Timer,0);
	}else{
		glutTimerFunc(0,_Timer,0);
	}
	return 0;
}

int Listener::Draw(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_BLEND);
	glLoadIdentity();
	gluLookAt(cam.eye.x, cam.eye.y, cam.eye.z , cam.o.x, cam.o.y, cam.o.z, 0.0, 1.0, 0.0);

	sim->Draw();

	glDisable(GL_BLEND);
	glutSwapBuffers();
	return 0;
}

int Listener::Mouse(int button,int state,int x,int y){
	switch (button) {
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN)	m.mbl=1;
		else					m.mbl=0;
		break;
	case GLUT_MIDDLE_BUTTON:
		if (state == GLUT_DOWN)	m.mbm=1;
		else					m.mbm=0;
		break;
	case GLUT_RIGHT_BUTTON:
		if (state == GLUT_DOWN)	m.mbr=1;
		else					m.mbr=0;
		break;
	case 0x0003:	//Wheel Roll Plus
		if(state==1){
			pdVertex3d v;
			v = cam.eye - cam.o;
			v*= cam.scale;
			cam.eye = cam.o + v;
		}
		break;
	case 0x0004:	//Wheel Roll Minus
		if(state==1){
			pdVertex3d v;
			v = cam.eye - cam.o;
			v/= cam.scale;
			cam.eye = cam.o + v;
		}
		break;
	default:
		break;
	}
	if(m.mbr || m.mbl || m.mbm){
		glutPostRedisplay();
	}
	m.ox=x;
	m.oy=y;
	m.ombl=m.mbl;
	m.ombr=m.mbr;
	m.ombm=m.mbm;
	return 0;
}

int Listener::Keyboard(unsigned char key){
	double cms=0.1;
	switch (key) {
	case VK_ESCAPE:
		glutLeaveMainLoop();
		break;
	case VK_RETURN:
		if(fullscreen){
			ChangeDisplaySettings(NULL, 0);
			glutPositionWindow(windowPosX,windowPosY);
			glutReshapeWindow(windowSizeX,windowSizeY);
		}else{
			glutFullScreen();
		}
		fullscreen=!fullscreen;
		break;
	case 'w':
		cam.eye.y+=cms;
		cam.o.y+=cms;
		cam.VertexToPolar();
		break;
	case 's':
		cam.eye.y-=cms;
		cam.o.y-=cms;
		cam.VertexToPolar();
		break;
	case 'a':
		cam.eye.x-=cms;
		cam.o.x-=cms;
		cam.VertexToPolar();
		break;
	case 'd':
		cam.eye.x+=cms;
		cam.o.x+=cms;
		cam.VertexToPolar();
		break;
	case 'z':
		enableGraphic=!enableGraphic;
	default:
		break;
	}
	return 0;
}

#endif