#ifndef GESTUREMATCH_H
#define GESTUREMATCH_H

#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include "hand.h"
#include <fstream>
#include <map>

using namespace std;
using namespace cv;

void TemplateInput(HandTemplate *palm, HandTemplate *fist, HandTemplate *left, HandTemplate *right, HandTemplate *up);
void TemplateMatch(HandTemplate temp, Hand *hand);
#endif