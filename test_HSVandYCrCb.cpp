#include "test_HSVandYCrCb.h"

int TestHsvAndYcrcb(){
	VideoCapture capture(0);
	capture.set(CV_CAP_PROP_SETTINGS, 1);
	Mat frame;
	Mat frameHSV, frameYCrCb;
	Mat resultYCrCb;
	Mat y, cr, cb;
	vector<Mat> channels;
	while (1){
		capture >> frame;
		flip(frame, frame, 1);

		cvtColor(frame, frameHSV, CV_BGR2HSV);
		cvtColor(frame, frameYCrCb, CV_BGR2YCrCb);
		resultYCrCb.create(frame.size(), CV_8UC1);
		//inRange(frameYCrCb, Scalar(0, 140, 100), Scalar(255, 175, 120), resultYCrCb);
		split(frameYCrCb, channels);
		y = channels.at(0);
		cr = channels.at(1);
		cb = channels.at(2);
		for (int j = 1; j < y.rows - 1; j++)
		{
			uchar* currentCr = cr.ptr< uchar>(j);
			uchar* currentCb = cb.ptr< uchar>(j);
			uchar* current = resultYCrCb.ptr< uchar>(j);
			for (int i = 1; i < y.cols - 1; i++)
			{
				if ((currentCr[i] > 137) && (currentCr[i] < 175) && (currentCb[i] > 100) && (currentCb[i] < 118))
					current[i] = 255;
				else
					current[i] = 0;
			}
		}


		imshow("ycrcb", resultYCrCb);

		if (waitKey(1) == 27) break;
	}
	return 0;
}