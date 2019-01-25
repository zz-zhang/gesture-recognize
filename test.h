#ifndef TEST_H
#define TEST_H

#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;

class testClass{
public:
	int num;
	testClass();
	~testClass(){};

};
void ChangeClass(testClass* temp);
int GetVideoFromCamera();
int GetImageFromFile();
void ArrayTest(InputArray _src, OutputArray _dst);
#endif