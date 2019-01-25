#ifndef DISTTRANSFORM_H	
#define DISTTRANSFORM_H

#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>  
#include <cmath>
#include "hand.h"
using namespace std;
using namespace cv;

#define ARM_RATIO 1.3

pair<Point, double> DetectInCircles(vector<Point> contour, Mat src, int* armx, int* army, int *armR2);
int DistTransform();
int GetDistance(int x1, int y1, int x2, int y2);
void DistTransform(InputArray _src, OutputArray _dst, Hand* hand);
void CutArm(InputArray _src, OutputArray _dst, vector<Point> contour, int handx, int handy, int armx, int army);
double GetSlope(int x1, int y1, int x2, int y2);
#endif