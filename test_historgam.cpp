#include "test_historgam.h"

void TestHistorgam(){
	CvCapture* capture = cvCreateCameraCapture(0);
	IplImage* hist_img;
	IplImage* src;
	while (1){
		src = cvQueryFrame(capture);

		IplImage* hsv = cvCreateImage(cvGetSize(src), 8, 3);
		cvCvtColor(src, hsv, CV_BGR2HSV);

		IplImage* h_plane = cvCreateImage(cvGetSize(src), 8, 1);
		IplImage* s_plane = cvCreateImage(cvGetSize(src), 8, 1);
		IplImage* v_plane = cvCreateImage(cvGetSize(src), 8, 1);
		IplImage* planes[] = { h_plane, s_plane };
		cvSplit(hsv, h_plane, s_plane, v_plane, 0);

		int h_bins = 30, s_bins = 32;
		CvHistogram* hist;
		{
			int hist_size[] = { h_bins, s_bins };
			float h_ranges[] = { 0, 180 };
			float s_ranges[] = { 0, 255 };
			float* ranges[] = { h_ranges, s_ranges };
			hist = cvCreateHist(2, hist_size, CV_HIST_ARRAY, ranges, 1);
		}
		cvCalcHist(planes, hist, 0, 0);
		cvNormalizeHist(hist, 1.0);

		int scale = 10;
		hist_img = cvCreateImage(cvSize(h_bins * scale, s_bins * scale), 8, 3);
		cvZero(hist_img);

		float max_value = 0;
		cvGetMinMaxHistValue(hist, 0, &max_value, 0, 0);

		for (int h = 0; h < h_bins; ++h){
			for (int s = 0; s < s_bins; ++s){
				float bin_val = cvQueryHistValue_2D(hist, h, s);
				int intensity = cvRound(bin_val * 255 / max_value);
				cvRectangle(hist_img, cvPoint(h * scale, s * scale),
					cvPoint((h + 1) * scale - 1, (s + 1) * scale - 1),
					CV_RGB(intensity, intensity, intensity),
					CV_FILLED);
			}
		}

		cvShowImage("source", src);
		cvShowImage("H-S Histogram", hist_img);
		if (waitKey(33) == 27) break;
	}
	cvReleaseImage(&src);
	cvReleaseImage(&hist_img);
	cvDestroyAllWindows();
	
	return;
}