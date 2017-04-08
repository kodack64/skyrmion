
#pragma once

/*unix debug*/
//#undef _MSC_VER

#define _CRT_SECURE_NO_DEPRECATE
#define DEF_SETUP_NAME "skyrmion_setup.ini"

#include <string>
#include <algorithm>
#include <cfloat>
#include <climits>
#include <ctime>
#include <cmath>
#include <complex>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <utility>
#include <vector>
#include <cassert>

#ifdef _OPENMP
#include <omp.h>
#endif

#ifdef _MSC_VER
#include <freeglut.h>
#include <conio.h>
#include <windows.h>
#include <crtdbg.h>
#else
#ifndef MAX_PATH
#define MAX_PATH 256
#endif
#endif

#define PI (2*acos(0.0))
#define EPS (1.0e-10)

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if (p) { delete (p);     (p)=NULL; } }
#endif    
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if (p) { delete[] (p);   (p)=NULL; } }
#endif

using namespace std;

