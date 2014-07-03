//
//  HandRectCutter.h
//  JajankenHunter
//
//  Created by akihito on 2014/06/21.
//
//

#ifndef __JajankenHunter__HandRectCutter__
#define __JajankenHunter__HandRectCutter__

#include <iostream>
#include "opencv2/opencv.hpp"
#include "Labeling.h"
#include "HandClip.h"

class HandRectCutter;
typedef std::shared_ptr<HandRectCutter> HandRectCutterRef;

class HandRectCutter {
public:
    HandRectCutter();
    ~HandRectCutter();
    
    static HandRectCutterRef create();
    
    std::vector<HandInfo> calcHandRect(cv::Mat& graySclaedImage);
};

#endif /* defined(__JajankenHunter__HandRectCutter__) */
