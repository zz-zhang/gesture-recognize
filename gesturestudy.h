#ifndef GESTURESTUDY
#define GESTURESTUDY

#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include "hand.h"
using namespace cv;
using namespace std;


int GestureStudyInit(const string filename);
void GestureStudyOutput(const string filename);
void GestureStudy(Hand *hand);
#endif