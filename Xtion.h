//
//  Xtion.h
//  JajankenHunter
//
//  Created by akihito on 2014/06/21.
//
//

#ifndef __JajankenHunter__Xtion__
#define __JajankenHunter__Xtion__

#include "OpenNI.h"
#include "opencv2/opencv.hpp"

class Xtion;
typedef std::shared_ptr<Xtion> XtionRef;

class Xtion
{
public:
    Xtion();
    ~Xtion();
    
    static XtionRef create();
    
    void update();
    cv::Mat getColorImage();
    cv::Mat getDepthImage();
    
private:
    openni::Device device;
    openni::VideoStream colorStream;
    openni::VideoStream depthStream;
    std::vector<openni::VideoStream*> streams;
    
    cv::Mat colorImage;
    cv::Mat depthImage;
    
    void updateColorImage();
    void updateDepthImage();
};

#endif /* defined(__JajankenHunter__Xtion__) */
