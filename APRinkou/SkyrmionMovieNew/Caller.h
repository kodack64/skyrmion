
#pragma once

#include "Listener.h"

class Caller{
private:
	static Listener* lis;
public:
	static bool Init(int argc,char** argv){
		lis=new Listener;
		glutInit(&argc,argv);
		glutInitWindowSize(320,320);
		glutInitWindowPosition(100,100);
		glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE,GLUT_ACTION_GLUTMAINLOOP_RETURNS);
		glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH);
		glutCreateWindow("SkyrmionSimulation Result -> Movie");
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_DEPTH_TEST);
		glutDisplayFunc(Caller::Disp);
		glutReshapeFunc(Caller::Reshape);
		glutTimerFunc(0,(Caller::Timer),0);
		glutKeyboardFunc(Caller::Keyboard);
		return lis->Init(argc,argv);
	}
	static void Disp(){
		lis->Disp();
	}
	static void Timer(int){
		glutTimerFunc(lis->Timer(),Caller::Timer,0);
	}
	static void Reshape(int w,int h){
		glViewport(0, 0, w, h);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(30.0, (double)w / (double)h, 1.0, 500.0);
		glMatrixMode(GL_MODELVIEW);
	}
	static void Keyboard(unsigned char key ,int x ,int y){
		lis->Keyboard(key);
	}
};
Listener* Caller::lis=NULL;
