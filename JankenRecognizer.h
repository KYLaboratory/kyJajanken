#pragma once

#include "HandRecognizer.h"
#include "HandClipper.h"

#include <vector>
#include <string>

using namespace std;
using namespace cv;

class JankenRecognizer
{
public:
	JankenRecognizer(void);
	~JankenRecognizer(void);

	void recognizeHandByImage(const Mat& image, EHAND& leftPersonResult, EHAND& rightPersonResult);
	void getHandRect(cv::Rect& leftHandRect, cv::Rect& rightHandRect);
	void initializeRecognizer();


private:
	HandClipper clipper;
	HandRecognizer recognizer;
};

