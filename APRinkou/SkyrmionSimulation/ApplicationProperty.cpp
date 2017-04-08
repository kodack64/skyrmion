
#include "ApplicationProperty.h"
#include "Common.h"

int AP::argc=0;
char** AP::argv=0;
string AP::setupFile;
string AP::myPath;
string AP::myDirectory;

#ifndef _MSC_VER
map <pair<string,string>,string > AP::setup;
#endif

int AP::Init(int _argc,char** _argv){
	argc=_argc;
	argv=_argv;

#ifdef _MSC_VER
	char path[MAX_PATH];
	GetModuleFileName(NULL,path,MAX_PATH);
	myPath=path;
#else
	myPath=argv[0];
#endif

	string::size_type divPath=myPath.find_last_of('\\',myPath.length());
	if(divPath==string::npos){
		divPath=myPath.find_last_of('/',myPath.length());
		assert(divPath!=string::npos);
	}
	myDirectory=myPath.substr(0,divPath+1);

#ifdef _MSC_VER
	if(argc>1){
		string drop=argv[1];
		if(drop.find('\\',0)!=string::npos){
			setupFile=drop;
		}else{
			setupFile=myDirectory+drop;
		}
	}else{
		setupFile=myDirectory+DEF_SETUP_NAME;
	}
#else
	if(argc<=1){
		cout << "Usage : [program] [setupfile]" << endl;
		return -1;
	}
	setupFile=myDirectory+argv[1];
	if(LoadSetup()==-1)return -1;
#endif

	printf("Setup File : %s\n",setupFile.c_str());
#ifdef _OPENMP
	printf("Procs : %d\n",omp_get_num_procs());
	omp_set_num_threads(omp_get_num_procs());
	printf("Threads : %d\n",omp_get_max_threads());
#endif
	return 0;
}

int AP::Close(){
	return 0;
}

#ifdef _MSC_VER
int AP::GetInt(string field,string key,int def){
	assert(setupFile!="");
	int rtn;
	char buf[MAX_PATH];
	char defstr[MAX_PATH];
	sprintf(defstr,"%d",def);
	GetPrivateProfileString(field.c_str(),key.c_str(),defstr,buf,MAX_PATH,setupFile.c_str());
	if(strcmp(buf,defstr)==0){
		WritePrivateProfileString(field.c_str(),key.c_str(),buf,setupFile.c_str());
	}
	sscanf(buf,"%d",&rtn);
	return rtn;
}
double AP::GetDouble(string field,string key,double def){
	assert(setupFile!="");
	double rtn;
	char buf[MAX_PATH];
	char defstr[MAX_PATH];
	sprintf(defstr,"%le",def);
	GetPrivateProfileString(field.c_str(),key.c_str(),defstr,buf,MAX_PATH,setupFile.c_str());
	if(strcmp(buf,defstr)==0){
		WritePrivateProfileString(field.c_str(),key.c_str(),buf,setupFile.c_str());
	}
	sscanf(buf,"%le",&rtn);
	return rtn;
}
string AP::GetString(string field,string key,string def){
	assert(setupFile!="");
	string rtn;
	char buf[MAX_PATH];
	char defstr[MAX_PATH];
	sprintf(defstr,"%s",def.c_str());
	GetPrivateProfileString(field.c_str(),key.c_str(),defstr,buf,MAX_PATH,setupFile.c_str());
	if(strcmp(buf,defstr)==0){
		WritePrivateProfileString(field.c_str(),key.c_str(),buf,setupFile.c_str());
	}
	rtn=buf;
	return rtn;
}
#else
int AP::GetInt(string field,string key,int def){
	assert(setupFile!="");
	if(setup.find(make_pair(field,key))!=setup.end()){
		int rtn;
		sscanf(setup[make_pair(field,key)].c_str(),"%d",&rtn);
		return rtn;
	}else{
		cout << "param not found : " << field << "::" << key << endl;
		return def;
	}
}
double AP::GetDouble(string field,string key,double def){
	assert(setupFile!="");
	if(setup.find(make_pair(field,key))!=setup.end()){
		double rtn;
		sscanf(setup[make_pair(field,key)].c_str(),"%le",&rtn);
		return rtn;
	}else{
		cout << "param not found : " << field << "::" << key << endl;
		return def;
	}
}
string AP::GetString(string field,string key,string def){
	assert(setupFile!="");
	if(setup.find(make_pair(field,key))!=setup.end()){
		return setup[make_pair(field,key)];
	}else{
		cout << "param not found : " << field << "::" << key << endl;
		return def;
	}
}
int AP::LoadSetup(){
	FILE* fp;
	fp=fopen(setupFile.c_str(),"r");
	if(fp==NULL){
		cout << "SetupFile not found" << endl;
		return -1;
	}
	char buf[256];
	string field;
	string key;
	string value;
	for(;fgets(buf,256,fp)!=0;){
		if(strlen(buf)==0)continue;
		string sbuf=buf;
		if(sbuf[0]=='['){
			string::size_type end=sbuf.find(']',0);
			if(end==string::npos){
				printf("%s",buf);
				assert(end!=string::npos);
			}
			field=sbuf.substr(1,end-1);
		}else{
			string::size_type eqpos=sbuf.find('=',0);
			if(eqpos==string::npos)continue;
			key=sbuf.substr(0,eqpos);
			value=sbuf.substr(eqpos+1,sbuf.length()-eqpos-2);
			setup[make_pair(field,key)]=value;
		}
	}
	fclose(fp);
	return 0;
}
#endif
