#ifndef TEST_HISTORGAM_H
#define TEST_HISTORGAM_H

#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;

#define cvQueryHistValue_2D( hist, idx0, idx1 )   cvGetReal2D( (hist)->bins, (idx0), (idx1) )

void TestHistorgam();

#endif