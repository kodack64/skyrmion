
#include "Caller.h"

int main(int argc,char** argv){
	if(!Caller::Init(argc,argv)){
		return 0;
	}
	glutMainLoop();
	return 0;
}
