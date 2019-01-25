#ifndef CANNY86_H
#define CANNY86_H

#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
using namespace std;
using namespace cv;

int Canny86();
int ConnectEdge(IplImage * src);
void CannyThreshold(int, void*);
#endif