#ifndef THIN_H
#define THIN_H

#include <opencv2/opencv.hpp>
#include <iostream>
using namespace std;
using namespace cv;

void thin(const InputArray _src, OutputArray _dst, const int iterations);

#endif