//
//  AuraGenerator.cpp
//  JajankenHunter
//
//  Created by akihito on 2014/09/20.
//
//

#include "AuraGenerator.h"

AuraGenerator::AuraGenerator()
{
    
}

AuraGenerator::~AuraGenerator()
{
    
}

cv::Mat AuraGenerator::generateAura(const cv::Mat& colorImage, const cv::Mat& depthImage)
{
    cv::Mat auraImage;
    
    if (colorImage.dims > 0)
    {
        cv::Mat maskImage;
        threshold(depthImage, maskImage, 200, 255, cv::THRESH_BINARY);
        
        cv::Mat extractImage(colorImage.size(), CV_8UC4, cv::Scalar(255,255,255,0));
        cv::Mat materialImage(colorImage.size(), CV_8UC4, cv::Scalar(0,255,255,255));
        materialImage.copyTo(extractImage, maskImage);
        
        //cv::GaussianBlur(extractImage, auraImage, cv::Size(51,101), 20, 20);
        cv::blur(extractImage, auraImage, cv::Size(31,71));
        
        cv::Mat colorAImage(colorImage.size(), CV_8UC4);
        cv::cvtColor(colorImage, colorAImage, CV_BGR2BGRA);
        
        colorAImage.copyTo(auraImage, maskImage);
    }
    
    return auraImage;
}
