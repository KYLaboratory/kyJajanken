#pragma once

#include "Labeling.h"
#include <math.h>

#include "HandInfo.h"
#include "CinderOpenCV.h"
#include "cinder/ImageIo.h"

using namespace cv;

class HandClipper
{
public:
	HandClipper(void);
	~HandClipper(void);

	
    static const int RIGHT_PERSON = false;
    static const int LEFT_PERSON =true;
	HandInfo leftHandInfo;
	HandInfo rightHandInfo;


	void clipHands(const Mat& image, Mat& leftPersonImage, Mat& rightPersonImage);
    
private:
	Mat rotateEx(const Mat& src, const double angle, const cv::Rect& rect) const;
	int calcMaxHeightEx(const Mat& image, int left, int right, int index, int& top, int& bottom) const;
	int calcMaxHeight(const Mat& image, int left, int right, int index, int& top, int& bottom) const;
	Vec4f calcHandAngle(const Mat& image, const cv::Rect& handRect) const;

	bool getHandsInfo(const Mat& binImage, HandInfo& leftInfo, HandInfo& rightInfo, Mat& labelImage) const;
	int getRightEdge(const Mat& binimg) const;
	Mat clipHand(const Mat& binimg, const HandInfo& hand) const;
};

