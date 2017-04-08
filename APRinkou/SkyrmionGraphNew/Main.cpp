
#include <iostream>
#include <cstdio>

using namespace std;

#pragma warning(disable:4996)

int main(int argc,char** argv){
	if(argc<=1){printf("no argument\n");return 0;}
	char outfile[1024];
	char outgfile[1024];

	FILE *fp;
	bool isCorrect=true;
	string str=argv[1];
	sprintf(outfile,"%s",str.c_str());
	sprintf(outgfile,"%s.eps",str.substr(0,str.find_first_of(".")));

	fp = _popen("gnuplot","w");
	if(fp==NULL){printf("cannot open gnuplot\n");return 0;}
	fputs("set xlabel 'time(sec)'\n",fp);
	fputs("set ylabel 'potential'\n",fp);
	fputs("set xrange [0:*]\n",fp);
	fputs("set yrange [*:*]\n",fp);
	// step time pot potave potdev potmax potmin
	fprintf(fp,"plot '%s' using 2:4 with lines ti 'average'\n",outfile);
	fprintf(fp,"replot '%s' using 2:5 with lines ti 'deviation'\n",outfile);
	fprintf(fp,"replot '%s' using 2:6 with lines ti 'max'\n",outfile);
	fprintf(fp,"replot '%s' using 2:7 with lines ti 'min'\n",outfile);
	fputs("set terminal postscript enhanced\n",fp);
	fprintf(fp,"set output '%s'\n",outgfile);
	fputs("replot",fp);
	fflush(fp);
	printf("push any key to exit");
	cin.get();
	_pclose(fp);
	return 0;
}