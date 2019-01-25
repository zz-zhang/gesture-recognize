#include "skinotsu.h"

void ThresholdOtsu(const InputArray _src, OutputArray _dst)//otsu 最大类间差分法，一种自适应阈值确定方法
{
	Mat src = _src.getMat();
	Mat dst(src.size(), src.type());
	dst = _dst.getMat();
	IplImage* srcIpl = cvCreateImage(cvSize(src.cols, src.rows), 8, 1);
	srcIpl->imageData = (char*)src.data;
	int height = srcIpl->height;
	int width = srcIpl->width;

	float histogram[256] = { 0 };
	for (int i = 0; i < height; i++)
	{
		unsigned char* p = (unsigned char*)srcIpl->imageData + srcIpl->widthStep * i;  //
		for (int j = 0; j < width; j++)
		{
			histogram[*p++]++;
		}
	}
	int size = height * width;
	for (int i = 0; i < 256; i++)
	{
		histogram[i] = histogram[i] / size;
	}
	float avgValue = 0;
	for (int i = 0; i < 256; i++)
	{
		avgValue += i*histogram[i];
	}

	int thresholdnum;
	float maxVariance = 0;
	float w = 0, u = 0;
	for (int i = 0; i < 256; i++)
	{
		w += histogram[i];
		u += i*histogram[i];
		float t = avgValue*w - u;
		float variance = t*t / (w*(1 - w));
		if (variance > maxVariance)
		{
			maxVariance = variance;
			thresholdnum = i;
		}
	}
	threshold(src, dst, thresholdnum, 255, CV_THRESH_BINARY);
	cvReleaseImage(&srcIpl);
}

void SkinOtsu(const InputArray _src, OutputArray _dst)
{
	Mat src = _src.getMat();
	_dst.create(src.size(), CV_8UC1);
	Mat dst = _dst.getMat();
	//assert(dst->nChannels == 1 && src->nChannels == 3);



	Mat ycrcb(src.size(), CV_8UC1);
	Mat cr(src.size(), CV_8UC3);
	Mat cb(src.size(), CV_8UC3);
	vector<Mat> channels;
	cvtColor(src, ycrcb, CV_BGR2YCrCb);
	split(ycrcb, channels);
	cr = channels.at(1);
	//imshow("cr", cr);
	ThresholdOtsu(cr, cr);

	

	cb = channels.at(2);
	//imshow("cb", cb);
	ThresholdOtsu(cb, cb);
	
	bitwise_xor(cr, cb, dst);
	cr.copyTo(dst);

	//waitKey(0);
}