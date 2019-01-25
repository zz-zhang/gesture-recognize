#include "hand.h"

Hand::Hand(){
	this->x = 0;
	this->y = 0;
	this->depth = 0;
	this->type = 0;
	this->prevX = 0;
	this->prevY = 0;
	this->prevType = 0;
	this->cirCenter = Point(0, 0);
	this->handCenter = Point(0, 0);
	this->defectsNum = 0;
	this->conCirRatio = 0;
	this->direction = 0;
	this->conRecRatio = 0;
	this->rectHWRatio = 0;
}


void Hand::Refresh(){
	this->prevX = this->x;
	this->prevY = this->y;
	this->prevType = this->type;
	this->x = 0;
	this->y = 0;
	this->defectsNum = 0;
	this->type = 0;
	this->conRecRatio = 0;
	this->rectHWRatio = 0;
}

void Hand::GetDirection(){
	if (this->cirCenter.x / this->handCenter.x < 1)
		this->direction = -1;
	else if (this->cirCenter.x / this->handCenter.x > 1)
		this->direction = 1;
	else this->direction = 0;
}