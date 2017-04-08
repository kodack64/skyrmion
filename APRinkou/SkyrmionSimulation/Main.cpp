
#include "Application.h"
#include "ApplicationProperty.h"

int main(int argc,char *argv[]){
	if(ApplicationProperty::Init(argc,argv)==-1){
		cout << "Setup fail"<<endl;
		return 0;
	}
	Application* application=new Application;
	if(application->Init()!=-1){
		application->Run();
	}
	application->Close();
	ApplicationProperty::Close();
	delete application;
	return 0;
}
