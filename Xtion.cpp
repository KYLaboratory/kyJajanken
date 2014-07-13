//
//  Xtion.cpp
//  JajankenHunter
//
//  Created by akihito on 2014/06/21.
//
//

#include "Xtion.h"

#pragma comment(lib,"C:\\Program Files (x86)\\OpenNI2\\Lib\\OpenNI2.lib")

Xtion::Xtion()
{
    try
    {
        openni::OpenNI::initialize();
        
        if(device.open(openni::ANY_DEVICE) != openni::STATUS_OK){
            throw std::exception();
        }
        
        colorStream.create(device, openni::SensorType::SENSOR_COLOR);
        depthStream.create(device, openni::SensorType::SENSOR_DEPTH);
        
        device.setDepthColorSyncEnabled(true);
        device.setImageRegistrationMode(openni::IMAGE_REGISTRATION_DEPTH_TO_COLOR);
        
        colorStream.start();
        depthStream.start();
        
        streams.push_back(&colorStream);
        streams.push_back(&depthStream);
    }
    catch (std::exception& e)
    {
        throw e;
    }
    
    depthImage = cv::Mat::zeros(depthStream.getVideoMode().getResolutionY()*2, depthStream.getVideoMode().getResolutionX()*2, CV_8U);
    
}

Xtion::~Xtion()
{
    colorStream.stop();
    depthStream.stop();
    
    colorStream.destroy();
    depthStream.destroy();
    
    device.close();
    
    openni::OpenNI::shutdown();
}

XtionRef Xtion::create()
{
    return XtionRef(new Xtion());
}

void Xtion::updateColorImage()
{
    openni::VideoFrameRef colorFrame;
    colorStream.readFrame(&colorFrame);
    if ( colorFrame.isValid() )
    {
        colorImage = cv::Mat( colorFrame.getHeight(), colorFrame.getWidth(), CV_8UC3, (char*)colorFrame.getData() );
        cv::cvtColor(colorImage, colorImage, CV_RGB2BGR);
        cv::resize(colorImage, colorImage, colorImage.size() * 2, cv::INTER_NEAREST);
    }
}

void Xtion::updateDepthImage()
{
    openni::VideoFrameRef depthFrame;
    depthStream.readFrame(&depthFrame);
    if (depthFrame.isValid())
    {
        openni::VideoMode videoMode = depthFrame.getVideoMode();
        depthImage = cv::Mat::zeros(videoMode.getResolutionY(),videoMode.getResolutionX(),CV_8UC1);
        openni::DepthPixel* depth = (openni::DepthPixel*)depthFrame.getData();
        const int maxDepth = 2000;
        for (int i = 0; i < (depthFrame.getDataSize()/sizeof(openni::DepthPixel)); ++i)
        {
            if (depth[i] != 0 && depth[i] < maxDepth)
            {
                depthImage.data[i] = ~(((depth[i] - depthStream.getMinPixelValue()) * 255) / (maxDepth - depthStream.getMinPixelValue()));
            }
        }
        cv::resize(depthImage, depthImage, depthImage.size() * 2, cv::INTER_NEAREST);
    }
}

void Xtion::update()
{
    int changedIndex = -1;
    openni::OpenNI::waitForAnyStream(&(streams[0]), (int)streams.size(), &changedIndex);
    switch (changedIndex)
    {
        case 0:
            updateColorImage();
            break;
        case 1:
            updateDepthImage();
            break;
        default:
            break;
    }
}

cv::Mat Xtion::getColorImage()
{
    return colorImage.clone();
}

cv::Mat Xtion::getDepthImage()
{
    return depthImage.clone();
}


