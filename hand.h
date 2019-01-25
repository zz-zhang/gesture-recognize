#ifndef HAND_H
#define HAND_H
#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;

#define GESTURE_INVALID 0
#define GESTURE_PALM 1
#define GESTURE_FIST 2
#define	GESTURE_LEFT 3
#define	GESTURE_RIGHT 4
#define	GESTURE_UP 5


class Hand{
public:	
	int x, y;			//掌心坐标
	int depth;			//掌心深度
	int type;			//手势类型
	int prevX, prevY;	//上一个手势坐标
	int prevType;
	Point2f cirCenter;	//外接圆圆心坐标
	Point2f handCenter;	//掌心坐标

	int defectsNum;		//有效凸缺陷数
	float conCirRatio;	//轮廓与外接圆面积比
	int direction;		//左右方向 -1 for left, 1 for right
	float conRecRatio;	//轮廓与外接矩形面积比
	float rectHWRatio;	//外接矩形长宽比

	Hand();
	~Hand(){};
	void Refresh();
	void GetDirection();
};

class HandTemplate{
public:
	int type;
	string typeStr;
	int defectsNumMin, defectsNumMax;
	float conCirRatioMin, conCirRatioMax;
	int directionMin, directionMax;
	float conRecRatioMin, conRecRatioMax;
	float rectHWRatioMin, rectHWRatioMax;
	HandTemplate(){}
	~HandTemplate(){}
};
#endif