#include "mpgcalc.h"


void MpgCalc(const InputArray _src, OutputArray _dst, Hand* hand){
	Mat src = _src.getMat();
	_dst.create(src.size(), src.type());
	Mat dst = _dst.getMat();

	threshold(src, src, 127, 255, THRESH_BINARY);

	Mat framergb(src.size(), CV_8UC3, Scalar(255, 255, 255));
	vector<vector<Point>> contours;
	Mat src2(src.size(), src.type());
	src.copyTo(src2);
	threshold(src2, src2, 127, 255, THRESH_BINARY);
	findContours(src2, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	if (contours.size() == 0) return;

	unsigned int maxPoint = 0;
	int indexContours;
	for (unsigned int i = 0; i < contours.size(); ++i){
		if (contours[i].size() > maxPoint){
			maxPoint = contours[i].size();
			indexContours = i;
		}
	}

	vector<vector<Point>> convex(contours.size());
	vector<vector<int>> convexI(contours.size());
	vector<vector<Vec4i>> defect(contours.size());
	for (unsigned int i = indexContours; i < contours.size(); ++i){
		convexHull(Mat(contours[i]), convex[i], false);
		convexHull(Mat(contours[i]), convexI[i], false);
		int size = convexI[i].size();
		if (convexI[i].size() > 3)
			convexityDefects(contours[i], convexI[i], defect[i]);
	}

	Point2f center;
	float radius;
	minEnclosingCircle(contours[indexContours], center, radius);

	//circle(framergb, center, radius, Scalar(85, 123, 205));
	//circle(framergb, center, 3, Scalar(85, 123, 205));

	hand->cirCenter = center;

	Mat dist_image(src.size(), CV_32FC1);
	distanceTransform(src, dist_image, CV_DIST_L2, 3);
	int temp = 0;
	int distMax = 0;
	for (int i = 0; i<src.rows; i++)
	for (int j = 0; j<src.cols; j++){

		if (src.ptr<uchar>(i)[j] > 200)
		{
			temp = (int)dist_image.ptr<float>(i)[j];
			if (temp > distMax)
			{
				distMax = temp;
				hand->handCenter.y = i;
				hand->handCenter.x = j;
			}

		}
	}
	//circle(framergb, hand->handCenter, 3, Scalar(0, 123, 0));
	hand->GetDirection();
	/*if (hand->direction == -1)
		cout << "left ";
		if (hand->direction == 1)
		cout << "right ";*/


	for (unsigned int i = indexContours; i < contours.size(); ++i){
		drawContours(framergb, contours, i, Scalar(150, 0, 0));
		drawContours(framergb, convex, i, Scalar(0, 0, 150));
		vector<Vec4i>::iterator d = defect[i].begin();
		int size = defect[i].size();
		while (d != defect[i].end()) {
			Vec4i& v = (*d);
			//if(IndexOfBiggestContour == i)
			{

				int startidx = v[0];
				Point ptStart(contours[i][startidx]); // point of the contour where the defect begins
				int endidx = v[1];
				Point ptEnd(contours[i][endidx]); // point of the contour where the defect ends
				int faridx = v[2];
				Point ptFar(contours[i][faridx]);// the farthest from the convex hull point within the defect
				int depth = v[3] / 256; // distance between the farthest point and the convex hull
				//cout << depth << " " << radius << endl;
				if ((float)depth / radius > DCDR)
				{
					line(framergb, ptStart, ptFar, Scalar(0, 0, 0), 1, CV_AA);
					line(framergb, ptEnd, ptFar, Scalar(0, 0, 0), 1, CV_AA);
					circle(framergb, ptStart, 1, Scalar(0, 0, 0), 2);
					circle(framergb, ptEnd, 1, Scalar(0, 0, 0), 2);
					circle(framergb, ptFar, 1, Scalar(0, 0, 0), 2);
					++hand->defectsNum;
				}

				/*printf("start(%d,%d) end(%d,%d), far(%d,%d)\n",
				ptStart.x, ptStart.y, ptEnd.x, ptEnd.y, ptFar.x, ptFar.y);*/
			}
			d++;
		}
	}

	hand->conCirRatio = contourArea(contours[indexContours]) / (PI * radius * radius);

	int up, down, left, right;
	up = left = 0xFFFF;
	down = right = 0;
	for (int i = 0; i < contours[indexContours].size(); ++i){
		Point p = contours[indexContours][i];
		up = min(up, p.y);
		left = min(left, p.x);
		down = max(down, p.y);
		right = max(right, p.x);
	}
	//rectangle(framergb, Point(left, up), Point(right, down), Scalar(200, 30, 150), 2);
	//RotatedRect rect = minAreaRect(contours[indexContours]);
	cout << down << " " << up << " " << right << " " << left << " ";
	hand->conRecRatio = contourArea(contours[indexContours]) / ((float)(down - up) * (float)(right - left));
	hand->rectHWRatio = (float)(down - up) / (float)(right - left);
	cout << hand->rectHWRatio << endl;
	//Point2f P[4];
	//rect.points(P);
	//for (int j = 0; j <= 3; j++)
	//{
	//	line(framergb, P[j], P[(j + 1) % 4], Scalar(200, 30, 150), 2);
	//}


	//cout << hand->defectsNum;
	//cout << hand->conCirRatio;
	//convexHull(contours[0], convex);
	//drawContours(frame, convex, -1, Scalar(150));
	imshow("convex", framergb);
	//framergb.copyTo(dst);
	//framergb.copyTo(dst);
}

