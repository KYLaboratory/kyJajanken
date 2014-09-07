#pragma once

#include "DP.h"
#include "Janken.h"
#include "CinderOpenCV.h"

using namespace cv;

class RecognizeResult{
public:
	int number;
	int answer;
	double rate;

};

class HandRecognizer
{
public:
	HandRecognizer(void);
	~HandRecognizer(void);
    
	static const int recognizeWidth = 40;
	static const int recognizeHeight = 30;

	EHAND recognize(const Mat& image);
    void addSample(const string& fileName, const EHAND& trick);

private:
	vector<cv::Mat> samples;
	vector<int> answers;
    DP<int> dpMatcher;
    
	static int countMatchingPix(const cv::Mat& img1, const cv::Mat& img2);
	vector<RecognizeResult> matchingAll(const cv::Mat& question);
    
	//Mat rightSideImage(Mat img);
	static vector<int> lineScan(const Mat& img);
	
};
