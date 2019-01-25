#ifndef AVGBACKGROUND_H
#define AVGBACKGROUND_H
#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;

void AllocateImages(Mat I);
void AccumulateBackground(Mat I);
void CreateModelsfromStats();
void SetHighThreshold(float scale);
void SetLowThreshold(float scale);
void BackgroundDiff(Mat I, IplImage* Imask);
int AvgBackground();
void DeallocateImage();
#endif