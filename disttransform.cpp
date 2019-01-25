#include "disttransform.h"
#include "hand.h"


int GetDistance(int x1, int y1, int x2, int y2){
	int dist;
	dist = pow((x1 - x2), 2) + pow((y1 - y2), 2);
	dist = sqrt(dist);
	return dist;
}

double GetSlope(int x1, int y1, int x2, int y2){
	if (x1 == x2)
		return 0.0;
	//else return (double)(y2 - y1) / (double)(x2 - x1);
	return	(double)(x2 - x1) / (double)(y2 - y1);
}

void DetectInCircles(vector<Point> contour, Mat src,int* handx,int* handy,int* R, int* armx, int* army, int *armR2)
{
	
	Mat dist_image(src.size(), CV_32FC1);
	distanceTransform(src, dist_image, CV_DIST_L2, 3);
	int temp = 0;
	int d;
	*R = 0;

	for (int i = 0; i<src.rows; i++)
	for (int j = 0; j<src.cols; j++){
		
		if (src.ptr<uchar>(i)[j] > 200)
		{
			temp = (int)dist_image.ptr<float>(i)[j];
			if (temp > *R)
			{
				*R = temp;
				*handy = i;
				*handx = j;
			}

		}
	}
	/*cout << *handx << " " << *handy << endl;
	cout << (int)src.ptr<uchar>(303)[143] << endl;
	cout << (int)dist_image.ptr<float>(303)[143];*/


	temp = 0;
	int R2 = *R * ARM_RATIO;
	int distance;
	
	for (int i = 0; i < src.rows; i++)
	for (int j = 0; j < src.cols; j++){
		if (src.ptr<uchar>(i)[j] > 200){
			distance = GetDistance(*handx, *handy, j, i);
			if (distance > R2 * 0.9 && distance < R2 * 1.1){
				temp = (int)dist_image.ptr<float>(i)[j];
				if (temp > *armR2){
					*armR2 = temp;
					*armx = j;
					*army = i;
				}
			}
		}
	}

	return;

}

void CutArm(InputArray _src, OutputArray _dst, vector<Point> contour, int handx, int handy, int armx, int army){
	Mat src = _src.getMat();
	_dst.create(src.size(), src.type());
	Mat dst = _dst.getMat();
	src.copyTo(dst);

	bool vertical = false;
	double slope;
	double anglePi;
	int angle;

	if (armx == handx) vertical = true; 
	else {
		slope = GetSlope(armx, army, handx, handy);
		anglePi = atan(slope);
		angle = anglePi * 180 / 3.14;
		//angle %= 90;
	}
	Point center;
	
	//cout << angle;

	//line(dst, Point(0, src.rows - 5), Point(src.cols - 1, src.rows - 5), Scalar(127));
	if (vertical){
		center.x = armx;
		center.y = src.rows - 1;
		circle(dst, center, GetDistance(armx, army, center.x, center.y), Scalar(0), CV_FILLED);
		rectangle(dst, Point(0, center.y), Point(src.cols - 1, src.rows - 1), Scalar(0), CV_FILLED);
	}
	else if (slope < 0){
		warpAffine(src, dst, getRotationMatrix2D(Point(handx, handy), -angle, 1), src.size());
		int nArmx, nArmy;
		nArmy = handy + GetDistance(handx, handy, armx, army);
		rectangle(dst, Point(0, nArmy), Point(dst.cols - 1, dst.rows - 1), Scalar(0), CV_FILLED);

		/*for (int i = 0; i < src.cols - 1; ++i){
			if (slope / GetSlope(handx, handy, i, src.rows - 1) < 1.1 &&
				slope / GetSlope(handx, handy, i, src.rows - 1) > 0.9){
				center.x = i;
				center.y = src.rows - 1;
				break;
			}
		}

		for (int i = src.rows - 1; i  >= 0; --i){
			if (slope / GetSlope(handx, handy, 0, i) < 1.1 &&
				slope / GetSlope(handx, handy, 0, i) > 0.9){
				center.x = 0;
				center.y = i;
				break;
			}
		}

		circle(dst, center, GetDistance(armx, army, center.x, center.y), Scalar(0), CV_FILLED);
		rectangle(dst, Point(0, center.y), Point(src.cols - 1, src.rows - 1), Scalar(0), CV_FILLED);*/
	}
	else if (slope > 0){
		warpAffine(src, dst, getRotationMatrix2D(Point(handx, handy), -angle, 1), src.size());
		int nArmx, nArmy;
		nArmy = handy + GetDistance(handx, handy, armx, army);
		rectangle(dst, Point(0, nArmy), Point(dst.cols - 1, dst.rows - 1), Scalar(0), CV_FILLED);
		/*for (int i = src.cols - 1; i >= 0; --i){
			if (slope / GetSlope(handx, handy, i, src.rows - 1) < 1.1 &&
				slope / GetSlope(handx, handy, i, src.rows - 1) > 0.9){
				center.x = i;
				center.y = src.rows - 1;
				break;
			}
		}

		for (int i = src.rows - 1; i >= 0; --i){
			if (slope / GetSlope(armx, army, src.cols - 1, i) < 1.1 &&
				slope / GetSlope(armx, army, src.cols - 1, i) > 0.9){
				center.x = src.cols - 1;
				center.y = i;
				break;
			}
			
		}
		circle(dst, center, GetDistance(armx, army, center.x, center.y), Scalar(0), CV_FILLED);
		rectangle(dst, Point(0, center.y), Point(src.cols - 1, src.rows - 1), Scalar(0), CV_FILLED);*/
	}
	//cout << center.x << " " << center.y << endl;
	//bool vertical = false;
	//bool left = false, right = false;
	//double slope;
	//if (army == handy){
	//	vertical = true; 
	//	if (armx < handx)
	//		left = true;
	//	else right = true;
	//}
	//else if (handx == armx)	slope = 0;
	//else{
	//	slope = (double)(army - handy) / (double)(armx - handy);
	//	slope /= -1;
	//}
	//int point1;
	//int point2;
	//if (vertical){			//army = handy
	//	if (left){			//armx < handx
	//		for (point1 = 0; point1 < contour.size(); ++point1)
	//		for (point2 = point1; point2 < contour.size(); ++point2){
	//			if (contour[point1].x < armx && contour[point1].x == contour[point2].x)
	//				line(dst, contour[point1], contour[point2], Scalar(0));
	//		}
	//	}
	//	else if (right){	//armx > handx
	//		for (point1 = 0; point1 < contour.size(); ++point1)
	//		for (point2 = point1; point2 < contour.size(); ++point2){
	//			if (contour[point1].x > armx && contour[point1].x == contour[point2].x)
	//				line(dst, contour[point1], contour[point2], Scalar(0));
	//		}
	//	}
	//}
	//else{					//army > handy
	//	for (point1 = 0; point1 < contour.size(); ++point1)
	//	for (point2 = point1; point2 < contour.size(); ++point2){
	//		double slopeLine = (double)(contour[point1].y - contour[point2].y) / (double)(contour[point1].x - contour[point2].x);
	//		if (slope > 0){		//armx < handx
	//			if ((abs(slope - slopeLine) < 0.001) && (contour[point1].y > army || contour[point2].y > army))
	//				line(dst, contour[point1], contour[point2], Scalar(0));
	//			if ((contour[point1].x < armx && contour[point1].y > army &&
	//				contour[point2].x < armx && contour[point2].y > army))
	//				rectangle(dst, contour[point1], contour[point2], Scalar(0));
	//		}
	//		else if (slope == 0){	//armx = handx
	//			if (contour[point1].y > army && contour[point1].y == contour[point2].y)
	//				line(dst, contour[point1], contour[point2], Scalar(0));
	//		}
	//		else{					//armx > handx
	//			if ((contour[point1].x > armx && contour[point1].y > army &&
	//				contour[point2].x > armx && contour[point2].y > army)
	//				|| (abs(slope - slopeLine) < 0.001) && (contour[point1].y < army || contour[point2].y < army))
	//				line(dst, contour[point1], contour[point2], Scalar(0));
	//		}
	//	}
	//}
}

void DistTransform(InputArray _src, OutputArray _dst, Hand* hand)
{
	Mat src = _src.getMat();
	_dst.create(src.size(), src.type());
	Mat dst = _dst.getMat();

	src.copyTo(dst);
	Mat image(src.size(), src.type());
	vector<vector<Point>> contours;


	//findContours的输入是二值图像    
	findContours(image,
		contours, // a vector of contours     
		CV_RETR_EXTERNAL, // retrieve the external contours    
		CV_CHAIN_APPROX_NONE); // retrieve all pixels of each contours    
	// Print contours' length轮廓的个数    
	//cout << "Contours: " << contours.size() << endl;
	vector<vector<Point>>::const_iterator itContours = contours.begin();
	/*
	for (; itContours != contours.end(); ++itContours) {

		cout << "Size: " << itContours->size() << endl;//每个轮廓包含的点数    
	}
	*/
	//找到最大轮廓  
	int index = 0, maxArea = 0;
	for (unsigned int i = 0; i<contours.size(); i++)
	{
		int area = contourArea(contours[i]);
		if (area > maxArea)
		{
			index = i;
			maxArea = area;
		}
	}
	
	if (contours.size()){
		int handx, handy, R;
		int armx, army, armR2;
		DetectInCircles(contours[index], src, &handx, &handy, &R, &armx, &army, &armR2);
		hand->x = handx;
		hand->y = handy;
		hand->depth = R;
		Point handPoint(handx, handy);
		Point armPoint(armx, army);
		//cout << m.first.x << " " << m.first.y << " " << m.second << endl;

		if (armR2 > R * 0.5)
			CutArm(src, dst, contours[index], handx, handy, armx, army);


		//src.copyTo(dst);
		//circle(dst, handPoint, 3, Scalar(127), 2);
		//circle(dst, armPoint, 3, Scalar(127), 2);
		//circle(dst, handPoint, R, Scalar(127), 1);
		//circle(dst, handPoint, R * ARM_RATIO, Scalar(127), 1);

		//circle(dst, Point(143, 303), 3, Scalar(127), 2);
		imshow("dist", dst);
		}
	return;
}