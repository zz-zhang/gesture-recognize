#ifndef SKINOTSU_H
#define SKINOTSU_H

#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
using namespace std;
using namespace cv;

void SkinOtsu(const InputArray _src, OutputArray _dst);
void ThresholdOtsu(const InputArray _src, OutputArray _dst);

#endif