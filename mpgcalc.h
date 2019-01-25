#ifndef MPGCALC_H
#define MPGCALC_H

#include <opencv2/opencv.hpp>
#include <iostream>
#include "hand.h"
using namespace std;
using namespace cv;
#define DCDR 0.2
#define PI 3.1415926

void MpgCalc(const InputArray _src, OutputArray _dst, Hand* hand);


#endif