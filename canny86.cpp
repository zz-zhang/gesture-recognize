#include "canny86.h"
/// 全局变量

Mat src, src_gray;
Mat dst, detected_edges;

int edgeThresh = 1;
int lowThreshold = 10;
int const max_lowThreshold = 100;
int ratio = 7;
int kernel_size = 3;
char* window_name = "Edge Map";

Mat canny3, canny5, canny10, binary;

/**
* @函数 CannyThreshold
* @简介： trackbar 交互回调 - Canny阈值输入比例1:3
*/
void CannyThreshold(int, void*)
{
	/// 使用 3x3内核降噪
	blur(src_gray, detected_edges, Size(3, 3));
	/// 运行Canny算子
	Canny(detected_edges, detected_edges, lowThreshold, lowThreshold*ratio, kernel_size);
	binary.create(detected_edges.size(), CV_8UC1);

	threshold(detected_edges, binary, 127, 255, THRESH_BINARY);
	imshow("binary", binary);
	//morphologyEx(binary, binary, MORPH_OPEN, getStructuringElement(MORPH_RECT, Size(1, 2)));
	//morphologyEx(binary, binary, MORPH_CLOSE, getStructuringElement(MORPH_RECT, Size(5, 5)));
	morphologyEx(binary, binary, MORPH_DILATE, getStructuringElement(MORPH_RECT, Size(2, 2)));

	IplImage *binaryIpl;
	binaryIpl = cvCreateImage(cvSize(binary.cols, binary.rows), 8, 1);
	binaryIpl->imageData = (char*)binary.data;
	ConnectEdge(binaryIpl);
	binary.data = (uchar*)binaryIpl->imageData;
	for (int i = 0; i < binary.rows; ++i){
		binary.ptr(i)[0] = 255;
		binary.ptr(i)[binary.cols - 1] = 255;
	}
	for (int j = 0; j < binary.cols; ++j){
		binary.ptr(0)[j] = 255;
		binary.ptr(binary.rows - 1)[j] = 255;
	}
	imshow("binary_open", binary);
	/// 使用 Canny算子输出边缘作为掩码显示原图像
	dst = Scalar::all(0);

	vector<vector<Point>> contours;
	findContours(binary, contours, CV_RETR_EXTERNAL, // retrieve the external contours  
		CV_CHAIN_APPROX_NONE);

	Mat contoursImage(binary.size(), binary.type(), Scalar(0));

	drawContours(contoursImage, contours, -1, Scalar(255), -1);
	imshow("contours", contoursImage);
	/*Canny(detected_edges, canny3, lowThreshold, lowThreshold , kernel_size);
	Canny(detected_edges, canny5, lowThreshold, lowThreshold * 5, kernel_size);
	Canny(detected_edges, canny10, lowThreshold, lowThreshold * 20, kernel_size);

	imshow("3", canny3);
	imshow("5", canny5);
	imshow("10", canny10);*/

	src.copyTo(dst, detected_edges);
	imshow(window_name, dst);

}


/** @函数 main */
int Canny86()
{
	/// 装载图像
	VideoCapture capture(0);
	while (1){
		capture >> src;
		flip(src, src, 1);
		if (!src.data)
		{
			return -1;
		}

		/// 创建与src同类型和大小的矩阵(dst)
		dst.create(src.size(), src.type());

		/// 原图像转换为灰度图像
		cvtColor(src, src_gray, CV_BGR2GRAY);

		/// 创建显示窗口
		namedWindow(window_name, CV_WINDOW_AUTOSIZE);

		/// 创建trackbar
		//createTrackbar("Min Threshold:", window_name, &lowThreshold, max_lowThreshold, CannyThreshold);

		/// 显示图像
		CannyThreshold(0, 0);

		/// 等待用户反应
		if (waitKey(33) == 27) break;
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////
//连接图像中断裂的边缘
//以某一点(i,j)为中心,分析它的八邻域
//////////////////////////////////////////////////////////////////////////
int ConnectEdge(IplImage * src)
{
	if (NULL == src)
		return 1;

	int width = src->width;
	int height = src->height;

	uchar * data = (uchar *)src->imageData;
	for (int i = 2; i < height - 2; i++)
	{
		for (int j = 2; j < width - 2; j++)
		{
			//如果该中心点为255,则考虑它的八邻域
			if (data[i * src->widthStep + j] == 255)
			{
				int num = 0;
				for (int k = -1; k < 2; k++)
				{
					for (int l = -1; l < 2; l++)
					{
						//如果八邻域中有灰度值为0的点，则去找该点的十六邻域
						if (k != 0 && l != 0 && data[(i + k) * src->widthStep + j + l] == 255)
							num++;
					}
				}
				//如果八邻域中只有一个点是255，说明该中心点为端点，则考虑他的十六邻域
				if (num == 1)
				{
					for (int k = -2; k < 3; k++)
					{
						for (int l = -2; l < 3; l++)
						{
							//如果该点的十六邻域中有255的点，则该点与中心点之间的点置为255
							if (!(k < 2 && k > -2 && l < 2 && l > -2) && data[(i + k) * src->widthStep + j + l] == 255)
							{
								data[(i + k / 2) * src->widthStep + j + l / 2] = 255;
							}
						}
					}
				}
			}
		}
	}
}
