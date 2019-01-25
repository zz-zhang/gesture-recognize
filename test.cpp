#include "test.h"

int GetImageFromFile(){
	Mat frame;
	VideoCapture capture(".\\frame\\frame0.jpg");
	if (!capture.isOpened()){
		cout << "camera error" << endl;
		return -1;
	}
	
	capture >> frame;
	//flip(frame, frame, 1);
	namedWindow("Image Window");
	imshow("Image Window", frame);
	waitKey(0);
	return 0;
}

int GetVideoFromCamera(){
	Mat frame;
	VideoCapture capture(0);
	if (!capture.isOpened()){
		cout << "camera error" << endl;
		return -1;
	}
	//capture.set(CV_CAP_PROP_SETTINGS, 1);
	while (1){
		capture >> frame;
		flip(frame, frame, 1);
		imshow("Video Window", frame);
		
		if (waitKey(33) == 27) break;
	}
	return 0;
}

void ArrayTest(InputArray _src, OutputArray _dst){
	Mat src = _src.getMat();
	_dst.create(src.size(), src.type());
	Mat dst = _dst.getMat();
	flip(src, dst, 1);
}

void ChangeClass(testClass* temp){
	temp->num = 9;
}

testClass::testClass(){
	this->num = 0;
}