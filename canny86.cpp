#include "canny86.h"
/// ȫ�ֱ���

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
* @���� CannyThreshold
* @��飺 trackbar �����ص� - Canny��ֵ�������1:3
*/
void CannyThreshold(int, void*)
{
	/// ʹ�� 3x3�ں˽���
	blur(src_gray, detected_edges, Size(3, 3));
	/// ����Canny����
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
	/// ʹ�� Canny���������Ե��Ϊ������ʾԭͼ��
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


/** @���� main */
int Canny86()
{
	/// װ��ͼ��
	VideoCapture capture(0);
	while (1){
		capture >> src;
		flip(src, src, 1);
		if (!src.data)
		{
			return -1;
		}

		/// ������srcͬ���ͺʹ�С�ľ���(dst)
		dst.create(src.size(), src.type());

		/// ԭͼ��ת��Ϊ�Ҷ�ͼ��
		cvtColor(src, src_gray, CV_BGR2GRAY);

		/// ������ʾ����
		namedWindow(window_name, CV_WINDOW_AUTOSIZE);

		/// ����trackbar
		//createTrackbar("Min Threshold:", window_name, &lowThreshold, max_lowThreshold, CannyThreshold);

		/// ��ʾͼ��
		CannyThreshold(0, 0);

		/// �ȴ��û���Ӧ
		if (waitKey(33) == 27) break;
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////
//����ͼ���ж��ѵı�Ե
//��ĳһ��(i,j)Ϊ����,�������İ�����
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
			//��������ĵ�Ϊ255,�������İ�����
			if (data[i * src->widthStep + j] == 255)
			{
				int num = 0;
				for (int k = -1; k < 2; k++)
				{
					for (int l = -1; l < 2; l++)
					{
						//������������лҶ�ֵΪ0�ĵ㣬��ȥ�Ҹõ��ʮ������
						if (k != 0 && l != 0 && data[(i + k) * src->widthStep + j + l] == 255)
							num++;
					}
				}
				//�����������ֻ��һ������255��˵�������ĵ�Ϊ�˵㣬��������ʮ������
				if (num == 1)
				{
					for (int k = -2; k < 3; k++)
					{
						for (int l = -2; l < 3; l++)
						{
							//����õ��ʮ����������255�ĵ㣬��õ������ĵ�֮��ĵ���Ϊ255
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
