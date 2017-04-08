
#include "Common.h"
#include "Listener.h"
#include "ApplicationProperty.h"

#ifdef _MSC_VER
Listener* Listener::listener = new Listener;

void __cdecl Listener::_Timer(int val){
	GetInstance()->Timer();
};

void __cdecl Listener::_Disp(void){
	GetInstance()->Draw();	
};

void __cdecl Listener::_Reshape(int w,int h){
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(30.0, (double)w / (double)h, 1.0, 500.0);
	glMatrixMode(GL_MODELVIEW);
}

void __cdecl Listener::_Mouse(int button,int state,int x,int y){
	GetInstance()->Mouse(button,state,x,y);
}

void __cdecl Listener::_Keyboard(unsigned char key,int x,int y){
	GetInstance()->Keyboard(key);
}

#endif