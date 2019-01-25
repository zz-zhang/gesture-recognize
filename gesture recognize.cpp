#include <opencv2/opencv.hpp>
#include <iostream>

#include "framediff.h"
#include "avgbackground.h"
#include "codebook.h"
#include "inversefilter.h"
#include "canny86.h"
#include "skinotsu.h"
#include "thin.h"
#include "hand.h"
#include "mpgcalc.h"
#include "gesturestudy.h"
#include "gesturematch.h"

#include "test.h"
#include "test_getframe.h"
#include "test_historgam.h"
#include "disttransform.h"
#include "test_HSVandYCrCb.h"
#include "test_keyboardresp.h"


#define CBFRAMENUM 50

#define TEST

#ifndef TEST
int main(){

	//VideoCapture capture(0);
	VideoCapture capture(".\\frame\\video.mp4");
	//capture.set(CV_CAP_PROP_SETTINGS, 1);
	if (!capture.isOpened()){
		cout << "Camera Error" << endl;
		return -1;
	}
	Mat frame;
	capture >> frame;
	flip(frame, frame, 1);
	resize(frame, frame, Size(frame.cols / 2, frame.rows / 2));

	IplImage*	rawImage;
	IplImage*	yuvImage;
	IplImage*	ImaskCodeBook;
	IplImage*	ImaskSkin;

	codeBook*	cB;
	unsigned	cbBounds[CHANNELS];
	uchar*		pColor;					//YUV pointer
	int			imageLen;
	int			nChannels = CHANNELS;
	int			minMod[CHANNELS];
	int			maxMod[CHANNELS];

	Hand hand1, hand2;

	HandTemplate palm;
	HandTemplate fist;
	HandTemplate left;
	HandTemplate right;
	HandTemplate up;
	TemplateInput(&palm, &fist, &left, &right, &up);


	Mat Imask(frame.rows, frame.cols, CV_8UC1);
	Mat codeBookMat(frame.rows, frame.cols, CV_8UC1);
	Mat skinMat;
	Mat frameHSV(frame.rows, frame.cols, CV_8UC1);

	rawImage = cvCreateImage(cvSize(frame.cols, frame.rows), 8, 3);
	rawImage->imageData = (char*)frame.data;								// ��Ӱ���л�ȡÿһ֡��ͼ��
	yuvImage = cvCreateImage(cvGetSize(rawImage), 8, 3);					// ��yuvImage ����һ����rawImage �ߴ���ͬ,8λ3ͨ��ͼ��
	ImaskCodeBook = cvCreateImage(cvGetSize(rawImage), IPL_DEPTH_8U, 1);	// ΪImaskCodeBook ����һ����rawImage �ߴ���ͬ,8λ��ͨ��ͼ��
	ImaskSkin = cvCreateImage(cvGetSize(rawImage), IPL_DEPTH_8U, 1);
	cvSet(ImaskCodeBook, cvScalar(255));									// ���õ�ͨ����������Ԫ��Ϊ255,����ʼ��Ϊ��ɫͼ��
	imageLen = rawImage->width * rawImage->height;							// Դͼ���������༴���ظ���
	cB = new codeBook[imageLen];											// �õ���ͼ��������Ŀ����һ����һ���뱾,�Ա��ÿ�����ؽ��д���
	for (int i = 0; i<imageLen; i++)
		cB[i].numEntries = 0;			// ��ʼ��ÿ���뱾����Ԫ��ĿΪ0����imageLen���뱾��ÿһ�����ض�Ӧһ���뱾
	for (int i = 0; i<nChannels; i++){
		cbBounds[i] = CBBOUNDS;				// ����ȷ����Ԫ��ͨ������ֵ
		minMod[i] = MINMOD;				// ���ڱ�����ֺ�����
		maxMod[i] = MAXMOD;				// ������ֵ�Դﵽ��õķָ�
	}


	/*��ʼ������Ƶÿһ֡ͼ��*/
	for (int num = 0; ; num++){								// û������ѭ����������ѭ��
		if (!capture.read(frame)) break;
		//capture >> frame;
		flip(frame, frame, 1);
		resize(frame, frame, Size(frame.cols / 2, frame.rows / 2));
		rawImage->imageData = (char*)frame.data;


		cvCvtColor(rawImage, yuvImage, CV_BGR2YCrCb);	// ɫ�ʿռ�ת��,��rawImage ת����YUVɫ�ʿռ�,�����yuvImage
		pColor = (uchar*)(yuvImage->imageData);

		if (num <= CBFRAMENUM)								// 50֡�ڽ��б���ѧϰ			
			CodeBoookLearn(yuvImage, pColor, cB, cbBounds, imageLen, nChannels);
		else if (num == CBFRAMENUM){											// ��50֡ʱ�������溯��,ɾ���뱾�г¾ɵ���Ԫ	
				for (int c = 0; c<imageLen; c++)
					cvclearStaleEntries(cB[c]);						// ���������뱾��ɾ��ÿһ���뱾�г¾ɵ���Ԫ
				cout << "Background Build Finished." << endl;
			}
		
		else{
			BgSub(yuvImage, ImaskCodeBook, pColor, cB, imageLen, nChannels, minMod, maxMod);
			cvconnectedComponents(ImaskCodeBook, 1, 4, NULL, NULL, NULL);
			Imask.data = (uchar*)ImaskCodeBook->imageData;
			// ��ͨ����������
			//imshow("codebook", Imask);
			//cvtColor(frame, frameHSV, CV_BGR2HSV);
			//Mat dstTemp1(frame.rows, frame.cols, CV_8UC1);
			//Mat dstTemp2(frame.rows, frame.cols, CV_8UC1);
			//inRange(frameHSV, Scalar(0, 30, 30), Scalar(40, 170, 256), dstTemp1);
			//inRange(frameHSV, Scalar(156, 30, 30), Scalar(180, 170, 256), dstTemp2);
			//bitwise_or(dstTemp1, dstTemp2, skinMat);
			//ImaskSkin->imageData = (char*)skinMat.data;
			//cvconnectedComponents(ImaskSkin, 1, 4, NULL, NULL, NULL);
			//skinMat.data = (uchar*)ImaskSkin->imageData;
			////��ɫ��ֵ+ȥ��

			//Mat codeBookSkin(frame.rows, frame.cols, CV_8UC1);
			//bitwise_and(skinMat, codeBookMat, codeBookSkin);

			SkinOtsu(frame, skinMat);
			ImaskSkin->imageData = (char*)skinMat.data;
			cvconnectedComponents(ImaskSkin, 1, 4, NULL, NULL, NULL);
			skinMat.data = (uchar*)ImaskSkin->imageData;
			//imshow("skin", skinMat);
			bitwise_and(skinMat, Imask, Imask);
			

			vector<vector<Point>> contours;
			Mat copyImask(Imask.size(), Imask.type());
			Imask.copyTo(copyImask);
			int conArea, maxArea = 0;
			int index;
			findContours(copyImask, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
			for (unsigned int i = 0; i < contours.size(); ++i){
				conArea = contourArea(contours[i]);
				if (conArea > maxArea){
					maxArea = conArea;
					index = i;
				}
			}
			if (contours.size() > 0){
				vector<vector<Point>>::iterator eraseIndex = contours.begin() + index;
				contours.erase(eraseIndex);
			}
			int maxAreaCopy = maxArea;
			maxArea = 0;
			index = 0;
			for (unsigned int i = 0; i < contours.size(); ++i){
				conArea = contourArea(contours[i]);
				if (conArea > maxArea){
					maxArea = conArea;
					index = i;
				}
			}
			if (contours.size() > 0){
				vector<vector<Point>>::iterator eraseIndex = contours.begin() + index;
				if ((double)maxArea >= (double)maxAreaCopy * 0.5)
				contours.erase(eraseIndex);
			}
			Mat conImask(copyImask.size(), copyImask.type(), Scalar(255));
			drawContours(conImask, contours, -1, Scalar(0), CV_FILLED);
			bitwise_and(conImask, Imask, Imask);
			//ֻ�������δ����������������ƶ��������

			Imask.copyTo(copyImask);
			vector<vector<Point>> handsContours;
			findContours(copyImask, handsContours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
			Mat contoursImage(copyImask.size(), copyImask.type(), Scalar(0));
			Mat armDel1(copyImask.size(), copyImask.type(), Scalar(0));
			Mat armDel2(copyImask.size(), copyImask.type(), Scalar(0));

			Mat armDel1rgb(copyImask.size(), CV_8UC3, Scalar(0));
			Mat armDel2rgb(copyImask.size(), CV_8UC3, Scalar(0));

			if (handsContours.size() > 0){
				drawContours(contoursImage, handsContours, 0, Scalar(255), CV_FILLED);
				DistTransform(contoursImage, armDel1, &hand1);
				if (handsContours.size() == 1){
					MpgCalc(armDel1, armDel1, &hand1);
					TemplateMatch(palm, &hand1);
					TemplateMatch(fist, &hand1);
					TemplateMatch(left, &hand1);
					TemplateMatch(right, &hand1);
					TemplateMatch(up, &hand1);
					hand1.Refresh();
				}
				//contoursImage.data = armDel1.data;
				
			}

			if (handsContours.size() > 1){
				drawContours(contoursImage, handsContours, 0, Scalar(0), CV_FILLED);
				drawContours(contoursImage, handsContours, 1, Scalar(255), CV_FILLED);
				DistTransform(contoursImage, armDel2, &hand2);
				//bitwise_or(armDel1, armDel2, contoursImage);
				if (hand1.x > hand2.x){
					Mat tempMat(armDel1.size(), armDel1.type());
					tempMat.data = armDel1.data;
					armDel1.data = armDel2.data;
					armDel2.data = tempMat.data;
					int tempInt;
					tempInt = hand1.x;
					hand1.x = hand2.x;
					hand2.x = tempInt;

					tempInt = hand1.y;
					hand1.y = hand2.y;
					hand2.y = tempInt;

					tempInt = hand1.depth;
					hand1.depth = hand2.depth;
					hand2.depth = tempInt;
				}
				MpgCalc(armDel1, armDel1, &hand1);
				MpgCalc(armDel2, armDel2, &hand2);
				TemplateMatch(palm, &hand1);
				TemplateMatch(fist, &hand1);
				TemplateMatch(left, &hand1);
				TemplateMatch(right, &hand1);
				TemplateMatch(up, &hand1);
				TemplateMatch(palm, &hand2);
				TemplateMatch(fist, &hand2);
				TemplateMatch(left, &hand2);
				TemplateMatch(right, &hand2);
				TemplateMatch(up, &hand2);
				//imshow("rgb", armDel2rgb);
				//cout << hand1.y << " " << hand2.y << endl;
				//cout << hand2.defectsNum << " " << hand2.conCirRatio << " " << hand2.direction << endl;
				cout << hand1.type << " " << hand2.type << endl;
				hand1.Refresh();
				hand2.Refresh();
			}
			

			/*imshow("arm1", armDel1);
			imshow("arm2", armDel2);*/
			bitwise_or(armDel1, armDel2, Imask);
			//drawContours(contoursImage, handsContours, -1, Scalar(255), CV_FILLED);
			//imshow("handscontours", contoursImage);

			/*
			Mat ImaskBGR;
			cvtColor(Imask, ImaskBGR, CV_GRAY2BGR);
			DistTransform(ImaskBGR, ImaskBGR);
			imshow("Imaskbgr", ImaskBGR);

			*/
			//Mat distImask;
			//DistTransform(Imask, distImask);

			//imshow("conimask", conImask);
			//imshow("Imask", Imask);
			//imshow("distimask", distImask);
			if (num >= 1000) num -= 500;
		}

			imshow("Raw", frame);


		




		//cvSetMouseCallback("Raw", cvmovecallback, (void*)rawImage);
							// ѭ����ʾͼƬ��������Ӱ��
		//imshow("SkinThreshold", skinMat);
		//imshow("CodeBook", codeBookMat);


		if (waitKey(0) == 27)									// ��ESC���˳�
			break;
	}

	/*�ͷ��ڴ棬���ٴ���*/
	//cvReleaseCapture(&capture);
	if (yuvImage)
		cvReleaseImage(&yuvImage);
	if (ImaskCodeBook)
		cvReleaseImage(&ImaskCodeBook);
	cvDestroyAllWindows();
	delete[] cB;
}
#endif

#ifdef TEST

int main(){
	const string filename = "right";
	GestureStudyInit(filename);
	for (int i = 1; i <= 8; ++i){
		char* numchar = new char;
		_itoa(i, numchar, 10);
		string numstring = numchar;
		string name = ".\\frame\\" + filename + "\\" + numstring + ".mp4";
		VideoCapture capture(name);
		if (!capture.isOpened()) continue;
		Mat frame;
		capture >> frame;
		resize(frame, frame, Size(frame.cols / 2, frame.rows / 2));
		Mat skinMat;
		Mat armDel;
		IplImage *ImaskSkin = cvCreateImage(cvSize(frame.cols, frame.rows), IPL_DEPTH_8U, 1);

		Hand hand1;
		while (1){
			if (!capture.read(frame)) break;
			resize(frame, frame, Size(frame.cols / 2, frame.rows / 2));
			SkinOtsu(frame, skinMat);
			ImaskSkin->imageData = (char*)skinMat.data;
			cvconnectedComponents(ImaskSkin, 1, 4, NULL, NULL, NULL);
			skinMat.data = (uchar*)ImaskSkin->imageData;
			cvtColor(frame, frame, CV_BGR2GRAY);
			DistTransform(skinMat, armDel, &hand1);
			MpgCalc(armDel, armDel, &hand1);
			//cout << hand1.rectHWRatio << " " << hand1.conCirRatio << endl;

			GestureStudy(&hand1);
			hand1.Refresh();

			imshow("frame", frame);
			if (waitKey(1) == 27) break;

		}
	}
	GestureStudyOutput(filename);

	//VideoCapture capture(".//frame//disttrans.jpg");
	//Mat frame;
	//Mat frameGray;
	//Hand hand1;
	//capture >> frame;
	//imshow("frame", frame);
	////DistTransform(frame, frame, &hand1);
	//cvtColor(frame, frameGray, CV_BGR2GRAY);
	//MpgCalc(frameGray, frameGray, &hand1);
	//waitKey(0);
	//cout << right.typeStr << endl;
	//TestHsvAndYcrcb();
	//DistTransform();
	//CodeBook();
	//Canny86();
	//GetFrame();
	//InverseFilter();
	//GetImageFromFile();
	//GetVideoFromCamera();
	//TestHistorgam();
	//TestKeyboard();
	//waitKey();
	//system("pause");
	return 0;
}
#endif