#include "framediff.h"
#define DELAY 100

void FrameDifference(){
	Mat frontFrame, backFrame, diffFrame;
	VideoCapture capture(0);
	if (!capture.isOpened()){
		cout << "camera error" << endl;
		return;
	}
	capture >> frontFrame;
	capture >> backFrame;
	while (1){
		capture >> backFrame;
		flip(backFrame, backFrame, 1);
		absdiff(backFrame, frontFrame, diffFrame);
		imshow("diffFrame", diffFrame);
		imshow("backFrame", backFrame);
		imshow("frontFrame", frontFrame);
		frontFrame = backFrame.clone();
		if (waitKey(DELAY) == 27)	break;
	}
	return;
	backFrame.release();
}

