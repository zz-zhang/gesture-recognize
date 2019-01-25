#include "mousecolor.h"

void dispplex(IplImage* img, char* var1, char* var2, char* var3, int x, int y)
{
	CvFont font;
	cvInitFont(&font, CV_FONT_HERSHEY_SCRIPT_COMPLEX, 1.0, 1.0);
	cvPutText(img, var1, cvPoint(x, y), &font, cvScalar(255, 255, 0));
	cvPutText(img, var2, cvPoint(x + 100, y), &font, cvScalar(255, 255, 0));
	cvPutText(img, var3, cvPoint(x + 200, y), &font, cvScalar(255, 255, 0));
	cvShowImage("Raw", img);

}
void cvmovecallback(int event, int x, int y, int flags, void* para)
{
	IplImage* img = (IplImage*)para;
	switch (event){
	case CV_EVENT_LBUTTONDOWN:
	{
								 uchar *rgb;
								 rgb = (uchar*)(img->imageData + y*(img->widthStep) + (img->nChannels)*x);
								 char var1[5];
								 _itoa((int)rgb[1], var1, 10);
								 char var2[5];
								 _itoa((int)rgb[2], var2, 10);
								 char var3[5];
								 _itoa((int)rgb[3], var3, 10);
								 dispplex(img, var1, var2, var3, x, y);
								 break;
	}
	default:break;
	}
}