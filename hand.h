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
	int x, y;			//��������
	int depth;			//�������
	int type;			//��������
	int prevX, prevY;	//��һ����������
	int prevType;
	Point2f cirCenter;	//���ԲԲ������
	Point2f handCenter;	//��������

	int defectsNum;		//��Ч͹ȱ����
	float conCirRatio;	//���������Բ�����
	int direction;		//���ҷ��� -1 for left, 1 for right
	float conRecRatio;	//��������Ӿ��������
	float rectHWRatio;	//��Ӿ��γ����

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