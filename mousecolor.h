#ifndef MOUSECOLOR_H
#define MOUSECOLOR_H

#include <opencv2/opencv.hpp>
#include <iostream>
using namespace std;
using namespace cv;

void dispplex(IplImage*img, char*var1, char*var2, char* var3, int x, int y);
void cvmovecallback(int event, int x, int y, int flags, void*para);

#endif