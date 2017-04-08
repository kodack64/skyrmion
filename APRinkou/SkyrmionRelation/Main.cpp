

#include <string>
#include <cstdio>
#include <vector>
#include <Windows.h>

using namespace std;

#ifdef _MSC_VER
#pragma warning(disable:4996)
#endif

int main(int argc,char** argv){
	if(argc<=1){printf("no argument\n");return false;}

	char outfile[1024];
	char relfile[1024];
	string str=argv[1];
	sprintf(outfile,"%s",str.c_str());
	sprintf(relfile,"%s.rel",str.substr(0,str.find_first_of(".")).c_str());

	char cdir[1024];
	GetCurrentDirectory(1024,cdir);

	printf("[Setup]\ncurrentdir=%s\ndatafile=%s\nrelfile=%s\n",cdir,outfile,relfile);

	FILE *fp,*fpp;
	fp=fopen(outfile,"r");
	if(fp==NULL){printf("out file not found\n");return false;}
	fpp=fopen(relfile,"w");
	if(fpp==NULL){printf("rel file cannot open\n");return false;}

	bool isCorrect=true;
	char buf[1024];
	int n;

	fgets(buf,1024,fp);
	fprintf(fpp,buf);
	fgets(buf,1024,fp);
	isCorrect=isCorrect&(sscanf_s(buf,"n = %d",&n)>0);
	fprintf(fpp,buf);


	vector <vector <double> > x;
	vector <vector <double> > y;
	vector <int> fr;

	int dfr;
	int di;
	double px,py;
	printf("[LoadStart]\n");
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
			if(di>=n){
				isCorrect=false;
			}else{
				tx[di]=px;
				ty[di]=py;
			}
		}
		if(isCorrect){
			x.push_back(tx);
			y.push_back(ty);
			fr.push_back(dfr);
		}
	}
	printf("[LoadEnd]\nframelength=%d\n",fr.size());
	fclose(fp);

	printf("[CalcRelationStart]\n");
	for(unsigned int f=0;f<fr.size();f++){
		fprintf(fpp,"# %d\n",fr[f]);
		for(int i=0;i<n;i++){
			double minr=n*n*100;
			int minj=0;
			double r;
			for(int j=0;j<n;j++){
				if(i==j)continue;
				r=hypot(x[f][i]-x[f][j],y[f][i]-y[f][j]);
				if(minr>r){
					minr=r;
					minj=j;
				}
			}
			fprintf(fpp,"%d %lf %lf\n",i,x[f][minj]-x[f][i],y[f][minj]-y[f][i]);
		}
	}
	fclose(fpp);
	printf("[CalcRelationEnd]\n");
	return true;
}
