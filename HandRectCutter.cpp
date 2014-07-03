//
//  HandRectCutter.cpp
//  JajankenHunter
//
//  Created by akihito on 2014/06/21.
//
//

#include "HandRectCutter.h"

HandRectCutter::HandRectCutter()
{
    
}

HandRectCutter::~HandRectCutter()
{
    
}

HandRectCutterRef HandRectCutter::create()
{
    return HandRectCutterRef(new HandRectCutter());
}

std::vector<HandInfo> HandRectCutter::calcHandRect(cv::Mat& grayScaledImage)
{
	//âÊëúÉfÅ[É^ÇÃì«çû
    cv::Mat src_img = grayScaledImage;
    
	cv::Mat imageResult;
    
	cv::Mat gray_img;
	cv::Mat bin_img;
	//cv::cvtColor(src_img, gray_img, CV_BGR2GRAY);
	cv::threshold(src_img, bin_img, 150, 255, cv::THRESH_BINARY);// LabelingÇÃåãâ ÇéÛÇØéÊÇÈ
    
	// ÉâÉxÉäÉìÉOåãâ ÇèoóÕÇ∑ÇÈÅAê^Ç¡îíÇ»èÛë‘Ç≈èâä˙âª
    cv::Mat out_img(bin_img.size(), CV_8UC1, cv::Scalar(255));
	cv::Mat label(bin_img.size(), CV_16SC1);
    
    // ÉâÉxÉäÉìÉOÇé¿é{ ÇQílâªÇµÇΩâÊëúÇ…ëŒÇµÇƒé¿çsÇ∑ÇÈÅB
    LabelingBS  labeling;
    labeling.Exec(bin_img.data, (short *)label.data, bin_img.cols, bin_img.rows, true, 1000);  //ëÂÇ´Ç≥2,000à»â∫ÇÃÉâÉxÉãÇÕêÿÇËéÃÇƒ
    
    // ÉâÉxÉãÇ≥ÇÍÇΩóÃàÊÇÇ–Ç∆Ç¬Ç∏Ç¬ï`âÊ
    for( int i = 0; i < labeling.GetNumOfRegions(); i++)
    {
        // ÉâÉxÉäÉìÉOåãâ Ç≈ÉCÉçÉCÉçÇ∑ÇÈÅB
        // ÉâÉxÉäÉìÉOåãâ ÇÃóÃàÊÇíäèoÇ∑ÇÈÅB
        cv::Mat labelarea;
        cv::compare(label, i + 1, labelarea, CV_CMP_EQ);
        // íäèoÇµÇΩóÃàÊÇ…ÉâÉìÉ_ÉÄÇ»êFÇê›íËÇµÇƒèoóÕâÊëúÇ…í«â¡ÅB
        cv::Mat color(src_img.size(), CV_8UC1, randomColor());
        color.copyTo(out_img, labelarea);
    }
    
	HandInfo leftHand;
	HandInfo rightHand;
    std::vector<HandInfo> handInfo;
    
	if(getHandsInfo(bin_img, leftHand, rightHand))
    {
        handInfo.push_back(leftHand);
        handInfo.push_back(rightHand);
    }
    
    return handInfo;
}