#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"

#include "CinderOpenCV.h"

#include "Xtion.h"
#include "AuraEffectGenerator.h"
#include "TrickEffectGenerator.h"
#include "HandRectCutter.h"
#include "TrickEstimater.h"
#include "KasuyaEffector.h"

using namespace ci;
using namespace ci::app;
using namespace std;

#define WINDOW_HEIGHT 480
#define WINDOW_WIDTH 640

class JajankenHunterApp : public AppNative
{
    XtionRef xtion;
    gl::TextureRef texture;
    
    HandRectCutterRef handRectCutter;
    TrickEstimaterRef trickEstimater;
    
    //list<AuraEffectGeneratorRef> auraEffectGenerator;
    //list<TrickEffectGeneratorRef> trickEffectGenerator;
    vector<KasuyaEffectorRef> kasuyaEffector;
    
    vector<HandInfo> handInfo;
    
    //int handCount;
    
  public:
    void prepareSettings(Settings* settings);
	void setup();
	void mouseDown( MouseEvent event );
	void update();
	void draw();
};

void JajankenHunterApp::prepareSettings(Settings* settings)
{
    settings->setWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
}

void JajankenHunterApp::setup()
{
    xtion = Xtion::create();
    handRectCutter = HandRectCutter::create();
    trickEstimater = TrickEstimater::create();
    /*
    handCount = 2;
    for (int i = 0; i < handCount; i++)
    {
        auraEffectGenerator.push_back(AuraEffectGenerator::create(WINDOW_WIDTH, WINDOW_HEIGHT));
        trickEffectGenerator.push_back(TrickEffectGenerator::create(WINDOW_WIDTH, WINDOW_HEIGHT));
    }
    */
    kasuyaEffector.push_back(KasuyaEffector::create(cv::Vec2f(100.0f,100.0f), Color(1.0f, 1.0f, 0.0f)));
    kasuyaEffector.push_back(KasuyaEffector::create(cv::Vec2f(400.0f,100.0f), Color(0.0f, 1.0f, 1.0f)));
}

void JajankenHunterApp::mouseDown( MouseEvent event )
{
    //auraEffectGenerator->setCenterCordinate(event.getPos());
}

string getTrickName(EHAND hand)
{
    switch (hand) {
        case eHAND_ROCK:
            return "ROCK";
        case eHAND_SCISSORS:
            return "SCISSORS";
        case eHAND_PAPER:
            return "PAPER";
        default:
            return "ERROR";
    }
}

void JajankenHunterApp::update()
{
    xtion->update();
    cv::Mat colorImage = xtion->getColorImage();
    cv::Mat depthImage = xtion->getDepthImage();
    
    cv::Mat morDepthImage = depthImage.clone();
    cv::Mat element5(5,5,CV_8U,cv::Scalar(1));
    int number_of_morpholgy = 2;
	for(int i = 0; i < number_of_morpholgy; i++){
		cv::morphologyEx(depthImage, morDepthImage, MORPH_CLOSE, element5);
		cv::morphologyEx(morDepthImage, morDepthImage, MORPH_OPEN, element5);
	}

    handInfo = handRectCutter->calcHandRect(depthImage);
    
    EHAND hand1 = eHAND_ERROR;
	EHAND hand2 = eHAND_ERROR;
    
    if(handInfo.size() > 0)
    {
        if(handInfo[0].handRect.width > 0 && handInfo[0].handRect.height > 0 && handInfo[1].handRect.width > 0 && handInfo[1].handRect.height > 0)
        {
            cv::rectangle(morDepthImage, handInfo[0].handRect, randomColor(), 3);
            cv::rectangle(morDepthImage, handInfo[1].handRect, randomColor(), 3);
            
            trickEstimater->initialize();
            hand1 = trickEstimater->estimateTrick(morDepthImage, handInfo[0].handRect);
            console() << "left" << "\t" << hand1 << std::endl;

            trickEstimater->initialize();
            hand2 = trickEstimater->estimateTrick(morDepthImage, handInfo[1].handRect);
            console() << "right" << "\t" << hand2 << std::endl;

            cv::Rect rect1(handInfo[0].handRect);
            kasuyaEffector[0]->setPos(cv::Vec2f(rect1.x + rect1.width / 2, rect1.y + rect1.height / 2));
            
            cv::Rect rect2(handInfo[1].handRect);
            kasuyaEffector[1]->setPos(cv::Vec2f(rect2.x + rect2.width / 2, rect2.y + rect2.height / 2));
            
            kasuyaEffector[0]->setTrick(hand1);
            kasuyaEffector[1]->setTrick(hand2);
            
            kasuyaEffector[0]->start();
            kasuyaEffector[1]->start();
        }
    }
    else
    {
        kasuyaEffector[0]->stop();
        kasuyaEffector[1]->stop();
    }
    
    kasuyaEffector[0]->update();
    kasuyaEffector[1]->update();
    
    cv::putText(morDepthImage, getTrickName(hand1), cv::Point(100, 50), CV_FONT_HERSHEY_PLAIN, 2, cv::Scalar(255,255,255));
    cv::putText(morDepthImage, getTrickName(hand2), cv::Point(300, 50), CV_FONT_HERSHEY_PLAIN, 2, cv::Scalar(255,255,255));
    
    texture = gl::Texture::create(fromOcv(colorImage));
    
    /*
    for(list<AuraEffectGeneratorRef>::iterator iter = auraEffectGenerator.begin(); iter != auraEffectGenerator.end(); iter++)
    {
        (*iter)->update();
    }
    */
    //trickEffectGenerator->start(ROCK);
    //trickEffectGenerator->update();
}

void JajankenHunterApp::draw()
{
    //gl::disableAlphaBlending();
	gl::clear( Color(0, 0, 0) );
    
    //gl::setMatricesWindow(getWindowWidth(), getWindowHeight());
    if( texture )
    {
        gl::draw(texture);
    }
    
    kasuyaEffector[0]->draw();
    kasuyaEffector[1]->draw();
    
    //gl::enableAlphaBlending();
    //gl::draw(trickEffectGenerator->getTexture());
    //gl::draw(auraEffectGenerator->getTexture());
}

CINDER_APP_NATIVE( JajankenHunterApp, RendererGl )
