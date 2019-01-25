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
	rawImage->imageData = (char*)frame.data;								// 从影像中获取每一帧的图像
	yuvImage = cvCreateImage(cvGetSize(rawImage), 8, 3);					// 给yuvImage 分配一个和rawImage 尺寸相同,8位3通道图像
	ImaskCodeBook = cvCreateImage(cvGetSize(rawImage), IPL_DEPTH_8U, 1);	// 为ImaskCodeBook 分配一个和rawImage 尺寸相同,8位单通道图像
	ImaskSkin = cvCreateImage(cvGetSize(rawImage), IPL_DEPTH_8U, 1);
	cvSet(ImaskCodeBook, cvScalar(255));									// 设置单通道数组所有元素为255,即初始化为白色图像
	imageLen = rawImage->width * rawImage->height;							// 源图像的面积，亦即像素个数
	cB = new codeBook[imageLen];											// 得到与图像像素数目长度一样的一组码本,以便对每个像素进行处理
	for (int i = 0; i<imageLen; i++)
		cB[i].numEntries = 0;			// 初始化每个码本的码元数目为0，共imageLen个码本，每一个像素对应一个码本
	for (int i = 0; i<nChannels; i++){
		cbBounds[i] = CBBOUNDS;				// 用于确定码元各通道的阈值
		minMod[i] = MINMOD;				// 用于背景差分函数中
		maxMod[i] = MAXMOD;				// 调整其值以达到最好的分割
	}


	/*开始处理视频每一帧图像*/
	for (int num = 0; ; num++){								// 没有跳出循环条件，死循环
		if (!capture.read(frame)) break;
		//capture >> frame;
		flip(frame, frame, 1);
		resize(frame, frame, Size(frame.cols / 2, frame.rows / 2));
		rawImage->imageData = (char*)frame.data;


		cvCvtColor(rawImage, yuvImage, CV_BGR2YCrCb);	// 色彩空间转换,将rawImage 转换到YUV色彩空间,输出到yuvImage
		pColor = (uchar*)(yuvImage->imageData);

		if (num <= CBFRAMENUM)								// 50帧内进行背景学习			
			CodeBoookLearn(yuvImage, pColor, cB, cbBounds, imageLen, nChannels);
		else if (num == CBFRAMENUM){											// 到50帧时调用下面函数,删除码本中陈旧的码元	
				for (int c = 0; c<imageLen; c++)
					cvclearStaleEntries(cB[c]);						// 遍历所有码本，删除每一个码本中陈旧的码元
				cout << "Background Build Finished." << endl;
			}
		
		else{
			BgSub(yuvImage, ImaskCodeBook, pColor, cB, imageLen, nChannels, minMod, maxMod);
			cvconnectedComponents(ImaskCodeBook, 1, 4, NULL, NULL, NULL);
			Imask.data = (uchar*)ImaskCodeBook->imageData;
			// 连通域法消除噪声
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
			////肤色阈值+去噪

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
			//只留下最大次大轮廓，减少其他移动物体干扰

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
							// 循环显示图片，即播放影像
		//imshow("SkinThreshold", skinMat);
		//imshow("CodeBook", codeBookMat);


		if (waitKey(0) == 27)									// 按ESC键退出
			break;
	}

	/*释放内存，销毁窗口*/
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