#ifndef INVERSEFILTER
#define INVERSEFILTER

#include <opencv2/opencv.hpp>
#include <stdlib.h>
#include <iostream>
#include <windef.h>
#include <highgui.h>

#include<math.h>
#include<time.h>

using namespace cv;
using namespace std;

#define SWAP(a,b) tempr=(a);(a)=(b);(b)=tempr
/*
# pragma comment(linker, "/NODEFAULTLIB:atlthunk.lib")
# pragma comment(linker, "/NODEFAULTLIB:LIBCMT")
# pragma comment(linker, "/NODEFAULTLIB:MSVCRTD")
*/
BOOL fourn(double * data/*psrc*/, unsigned long nn[]/*w*/, int ndim/*2*/, int isign);
void InverseFilter();
#endif