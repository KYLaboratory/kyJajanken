//
//  Aura.cpp
//  JajankenHunter
//
//  Created by akihito on 2014/06/20.
//
//

#include "AuraEffectGenerator.h"
#include "cinder/gl/gl.h"
#include "CinderOpenCV.h"

using namespace ci;

AuraEffectGenerator::AuraEffectGenerator(const int width, const int height)
: width(width), height(height),
centerCordinate(0.0f, 0.0f)
{
}

AuraEffectGenerator::~AuraEffectGenerator()
{
    
}

AuraEffectGeneratorRef AuraEffectGenerator::create(const int width, const int height)
{
    return AuraEffectGeneratorRef(new AuraEffectGenerator(width, height));
}

void AuraEffectGenerator::setCenterCordinate(const Vec2i& cordinate)
{
    centerCordinate = cordinate;
}

void AuraEffectGenerator::update()
{
    cv::Mat matData = cv::Mat::zeros(height, width, CV_8UC4);
    cv::circle(matData, toOcv(centerCordinate), 100, cv::Scalar(200,240,255,100), -1, CV_AA);
    cv::blur(matData, matData, cv::Size(40,40));
    
    mTexture = gl::Texture::create(fromOcv(matData));
}

gl::TextureRef AuraEffectGenerator::getTexture()
{
    return mTexture;
}