//
//  AuraGenerator.h
//  JajankenHunter
//
//  Created by akihito on 2014/09/20.
//
//

#ifndef __JajankenHunter__AuraGenerator__
#define __JajankenHunter__AuraGenerator__

#include "CinderOpenCV.h"

class AuraGenerator
{
public:
    AuraGenerator();
    virtual ~AuraGenerator();
    
    cv::Mat generateAura(const cv::Mat& colorImage, const cv::Mat& depthImage);
    
private:
};



#endif /* defined(__JajankenHunter__AuraGenerator__) */
