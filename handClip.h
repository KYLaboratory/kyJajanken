#ifndef HANDCLIP_H
#define HANDCLIP_H

#include<vector>
#include "Labeling.h"
#include<iostream>
#include "opencv2/opencv.hpp"

using namespace std;

int getMaxHeight(cv::Mat image, int left, int right, int index, int& top, int& bottom);

cv::Scalar randomColor();

class HandInfo{
public:
	cv::Rect handRect;
	int personLabel;
	HandInfo();
	HandInfo(int left, int top, int right, int bottom, bool isLeftPerson);
	
	bool isLeftPersonHand();
	int getLeft();
	int getRight();
};

bool getHandsInfo(cv::Mat binImage, HandInfo& leftInfo, HandInfo& rightInfo);

#endif