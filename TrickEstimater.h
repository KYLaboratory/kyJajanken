//
//  TrickEstimater.h
//  JajankenHunter
//
//  Created by akihito on 2014/06/21.
//
//

#ifndef __JajankenHunter__TrickEstimater__
#define __JajankenHunter__TrickEstimater__

#include <iostream>
#include "opencv2/opencv.hpp"
#include "Janken.h"
#include "Labeling.h"

class TrickEstimater;
typedef std::shared_ptr<TrickEstimater> TrickEstimaterRef;

#define NUMBER_OF_HANDS 2
#define NUMBER_OF_CONTOUR_COORDINATE 2000

using namespace cv;

class TrickEstimater {
    
public:
    TrickEstimater();
    ~TrickEstimater();
    
    static TrickEstimaterRef create();
    void initialize();
    EHAND estimateTrick(const cv::Mat& image, const cv::Rect& rect);
    
private:
    vector<vector<cv::Point>> contours;
    
    double calcNorm(double rv_x1, double rv_y1, double rv_x2, double rv_y2);
    double calcInnerProduct(double rv_x1, double rv_y1, double rv_x2, double rv_y2, double rv_x3, double rv_y3);
    double calcOuterProduct(double rv_x1, double rv_y1, double rv_x2, double rv_y2, double rv_x3, double rv_y3);
    void labelContour(unsigned int contour_id, unsigned int rv_size);
    double getContourAreaMax(cv::vector<cv::vector<cv::Point>> rv_contours, cv::vector<cv::Vec4i> rv_hierarchy);
    void calcContourFeature();
    bool isSharpAngle(const double rv_theta);
    EHAND judgeScissorsPaperRock(const cv::Mat& rv_src_img);
};

#endif /* defined(__JajankenHunter__TrickEstimater__) */
