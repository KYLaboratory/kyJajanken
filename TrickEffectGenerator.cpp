//
//  TrickEffectGenerator.cpp
//  JajankenHunter
//
//  Created by akihito on 2014/06/20.
//
//

#include "TrickEffectGenerator.h"
#include "cinder/gl/gl.h"
#include "CinderOpenCV.h"

using namespace ci;

TrickEffectGenerator::TrickEffectGenerator(const int width, const int height)
: width(width), height(height),memberID(1),
trick(NONE), textPosition(width / 2, height / 2)
{
    
}

TrickEffectGenerator::~TrickEffectGenerator()
{
    
}

TrickEffectGeneratorRef TrickEffectGenerator::create(const int width, const int height)
{
    return TrickEffectGeneratorRef(new TrickEffectGenerator(width, height));
}

void TrickEffectGenerator::start(TRICK trick)
{
    this->trick = trick;
}

void TrickEffectGenerator::update()
{
    cv::Mat matData = cv::Mat::zeros(height, width, CV_8UC4);
    std::string text = "";
    switch (trick) {
        case ROCK:
            text = "ROCK";
            break;
        case SCISSORS:
            text = "SCISSORS";
            break;
        case PAPER:
            text = "PAPER";
            break;
        default:
            break;
    }
    cv::putText(matData, text, toOcv(textPosition), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0,0,0,100), 2, CV_AA);
    mTexture = gl::Texture::create(fromOcv(matData));
}

gl::TextureRef TrickEffectGenerator::getTexture()
{
    return mTexture;
}
