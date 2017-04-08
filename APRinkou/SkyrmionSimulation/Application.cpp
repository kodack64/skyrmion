
#include "Application.h"
#include "ApplicationProperty.h"
#include "SkyrmionSimulator.h"
#include "SkyrmionRungSimulator.h"
#include "ParticleCommon.h"

#ifdef _MSC_VER
#include "Listener.h"
#endif

int Application::Init(){

	srand((unsigned)time(NULL));

	sim = new SkyrmionRungSimulator;
	sim->Init();

#ifdef _MSC_VER
	Listener::GetInstance()->Init(sim);
#endif

	return 0;
}

int Application::Run(){
#ifdef _MSC_VER
	glutMainLoop();
#else
	for(;;){
		sim->WriteData();
		if(sim->Step()==-1){
			printf("End\n");
			break;
		}
	}
#endif
	return 0;
}

int Application::Close(){
	SAFE_DELETE(sim);
	return 0;
}

