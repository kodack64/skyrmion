
#pragma once

#include "Common.h"

#ifndef _MSC_VER
class SetupData{
public:
	SetupData(const string _field,const string _key,const string _value){
		field=_field;
		key=_key;
		value=_value;
	}
	SetupData(const char* _field,const char* _key,int _value){
		field=_field;
		key=_key;
		valuei=_value;
		isInteger=true;
	}
	SetupData(const char* _field,const char* _key,double _value){
		field=_field;
		key=_key;
		valued=_value;
		isInteger=false;
	}
/*	bool operator==(SetupData d) {
		return 
			(d.field==this->field)&&
			(d.key==this->key);
	}
	bool operator<(SetupData d) {
		if(field<d.field)return true;
		if(field>d.field)return false;
		if(key<d.key)return true;
		if(key>d.key)return false;
		return false;
	}*/

	string field;
	string key;
	string value;
	int valuei;
	double valued;
	bool isInteger;
};
#endif

class ApplicationProperty{
private:
	static string setupFile;
	static string myPath;
	static string myDirectory;
#ifndef _MSC_VER
	static int LoadSetup();
	static map <pair<string,string> ,string > setup;
#endif

public:
	static int argc;
	static char** argv;
	static int Init(int argc,char** argv);
	static int Close();

	static int GetInt(string field,string key,int def);
	static double GetDouble(string field,string key,double def);
	static string GetString(string field,string key,string def);

	static int GetArgc(){return argc;};
	static char** GetArgv(){return argv;};
};

#define AP ApplicationProperty

