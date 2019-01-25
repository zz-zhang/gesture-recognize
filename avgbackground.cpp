#include "avgbackground.h"

IplImage *IavgF, *IdiffF, *IprevF, *IhiF, *IlowF;
IplImage *Iscratch, *Iscratch2;
IplImage *Igray1, *Igray2, *Igray3;
IplImage *Ilow1, *Ilow2, *Ilow3;
IplImage *Ihi1, *Ihi2, *Ihi3;
IplImage *Imaskt, *Imask;
double Icount;



int AvgBackground(){
	VideoCapture capture(0);
	if (!capture.isOpened()){
		cout << "open error" << endl;
		return -1;
	}

	Mat img, imask;
	Mat *ImaskMat;
	capture >> img;
	flip(img, img, 1);
	imshow("原图", img);
	AllocateImages(img);
	cout << "开始统计背景模型" << endl;
	while (Icount < 300.0){
		AccumulateBackground(img);
		capture >> img;
		flip(img, img, 1);
		waitKey(33);
		imshow("原图", img);
		cout << ".";
	}

	CreateModelsfromStats();
	cout << endl << "统计背景模型结束" << endl;
	cout << "按任意键开始分割图像" << endl;
	//waitKey(NULL);
	cout << "开始分割图像" << endl;
	while (1){
		capture >> img;
		flip(img, img, 1);
		BackgroundDiff(img, Imask);
		ImaskMat = new cv::Mat(Imask, 0);
		imask = *ImaskMat;
		imshow("原图", img);
		imshow("检测图", imask);
		if (waitKey(33) == 27) break;
	}
	return 0;
}

void AllocateImages(Mat I){
	IplImage *IIpl = new IplImage(I);
	CvSize sz = cvGetSize(IIpl);
	IavgF = cvCreateImage(sz, IPL_DEPTH_32F, 3);
	IdiffF = cvCreateImage(sz, IPL_DEPTH_32F, 3);
	IprevF = cvCreateImage(sz, IPL_DEPTH_32F, 3);
	IhiF = cvCreateImage(sz, IPL_DEPTH_32F, 3);
	IlowF = cvCreateImage(sz, IPL_DEPTH_32F, 3);
	Ilow1 = cvCreateImage(sz, IPL_DEPTH_32F, 1);
	Ilow2 = cvCreateImage(sz, IPL_DEPTH_32F, 1);
	Ilow3 = cvCreateImage(sz, IPL_DEPTH_32F, 1);
	Ihi1 = cvCreateImage(sz, IPL_DEPTH_32F, 1);
	Ihi2 = cvCreateImage(sz, IPL_DEPTH_32F, 1);
	Ihi3 = cvCreateImage(sz, IPL_DEPTH_32F, 1);
	cvZero(IavgF);
	cvZero(IdiffF);
	cvZero(IprevF);
	cvZero(IhiF);
	cvZero(IlowF);
	cvZero(Ilow1);
	cvZero(Ilow2);
	cvZero(Ilow3);
	cvZero(Ihi1);
	cvZero(Ihi2);
	cvZero(Ihi3);

	Icount = 0.00001;

	Iscratch = cvCreateImage(sz, IPL_DEPTH_32F, 3);
	Iscratch2 = cvCreateImage(sz, IPL_DEPTH_32F, 3);
	Igray1 = cvCreateImage(sz, IPL_DEPTH_32F, 1);
	Igray2 = cvCreateImage(sz, IPL_DEPTH_32F, 1);
	Igray3 = cvCreateImage(sz, IPL_DEPTH_32F, 1);
	Imaskt = cvCreateImage(sz, IPL_DEPTH_8U, 1);
	Imask = cvCreateImage(sz, IPL_DEPTH_8U, 1);
	cvZero(Iscratch);
	cvZero(Iscratch2);
	cvZero(Imask);
	cvZero(Imaskt);
}

void AccumulateBackground(Mat I){
	IplImage *IIpl = new IplImage(I);
	static int first = 1;
	cvCvtScale(IIpl, Iscratch, 1, 0);
	if (!first){
		cvAcc(Iscratch, IavgF);
		cvAbsDiff(Iscratch, IprevF, Iscratch2);
		cvAcc(Iscratch2, IdiffF);
		Icount += 1.0;
	}
	first = 0;
	cvCopy(Iscratch, IprevF);
}

void CreateModelsfromStats(){
	cvConvertScale(IavgF, IavgF, (double)(1.0 / Icount));
	cvConvertScale(IdiffF, IdiffF, (double)(1.0 / Icount));

	cvAddS(IdiffF, cvScalar( 1.0, 1.0, 1.0), IdiffF);
	SetHighThreshold(7.0);
	SetLowThreshold(6.0);
}

void SetHighThreshold(float scale){
	cvConvertScale(IdiffF, Iscratch, scale);
	cvAdd(Iscratch, IavgF, IhiF);
	cvSplit(IhiF, Ihi1, Ihi2, Ihi3, 0);
}

void SetLowThreshold(float scale){
	cvConvertScale(IdiffF, Iscratch, scale);
	cvAdd(IavgF, Iscratch, IlowF);
	cvSplit(IlowF, Ilow1, Ilow2, Ilow3, 0);
}

void BackgroundDiff(Mat I, IplImage* Imask){
	IplImage *IIpl = new IplImage(I);
	cvCvtScale(IIpl, Iscratch, 1, 0);
	cvSplit(Iscratch, Igray1, Igray2, Igray3, 0);

	cvInRange(Igray1, Ilow1, Ihi1, Imask);

	cvInRange(Igray2, Ilow2, Ihi2, Imaskt);
	cvOr(Imask, Imaskt, Imask);

	cvInRange(Igray3, Ilow3, Ihi3, Imaskt);
	cvOr(Imask, Imaskt, Imask);

	cvSubRS(Imask, cvScalar(255), Imask);
}

void DeallocateImage(){
	cvReleaseImage(&IavgF);
	cvReleaseImage(&IdiffF);
	cvReleaseImage(&IprevF);
	cvReleaseImage(&IhiF);
	cvReleaseImage(&IlowF);
	cvReleaseImage(&Ilow1);
	cvReleaseImage(&Ilow2);
	cvReleaseImage(&Ilow3);
	cvReleaseImage(&Ihi1);
	cvReleaseImage(&Ihi2);
	cvReleaseImage(&Ihi3);
	cvReleaseImage(&Iscratch);
	cvReleaseImage(&Iscratch2);
	cvReleaseImage(&Igray1);
	cvReleaseImage(&Igray2);
	cvReleaseImage(&Igray3);
	cvReleaseImage(&Imaskt);
	cvReleaseImage(&Imask);
}