#include "test_getframe.h"

void GetFrame(){
	Mat frame;
	VideoCapture capture(0);
	if (!capture.isOpened())	return;
	int num = 0;
	while (1){
		capture >> frame;
		flip(frame, frame, 1);
		imshow("test_getframe", frame);
		char c = waitKey(33);
		if (c == 27)	break;
		else if (c == 13){
			string name, numstring;
			char* numchar = new char;
			_itoa(num, numchar, 10);
			numstring = numchar;
			name = ".\\frame\\frame" +  numstring + ".jpg";
			++num;
			imwrite(name, frame);
		}
	}
	return;
}