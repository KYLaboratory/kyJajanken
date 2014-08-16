#include "cinder/app/AppNative.h"
#include "cinder/gl/Texture.h"

#include "Xtion.h"

//#include "AuraEffectGenerator.h"
//#include "TrickEffectGenerator.h"

//#include "HandRectCutter.h"
//#include "TrickEstimater.h"
#include "KasuyaEffector.h"
#include "JankenRecognizer.h"

using namespace ci;
using namespace ci::app;
using namespace std;

#define WINDOW_HEIGHT 480
#define WINDOW_WIDTH 640

class JajankenHunterApp : public AppNative
{
    XtionRef xtion;
    gl::TextureRef texture;
    
    //HandRectCutterRef handRectCutter;
    //TrickEstimaterRef trickEstimater;
    
    //list<AuraEffectGeneratorRef> auraEffectGenerator;
    //list<TrickEffectGeneratorRef> trickEffectGenerator;
    vector<KasuyaEffectorRef> kasuyaEffector;
    
    vector<HandInfo> handInfo;
    
    JankenRecognizer recognizer;
    
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
    //handRectCutter = HandRectCutter::create();
    recognizer.initializeRecognizer();
    
    //trickEstimater = TrickEstimater::create();
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

string getTrickName(const EHAND& hand)
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

cv::Mat closeAndOpen(const cv::Mat& inputImage)
{
    cv::Mat morImage = inputImage.clone();
    const cv::Mat element5(5,5,CV_8U,cv::Scalar(1));
    const int number_of_morpholgy = 2;
	for(int i = 0; i < number_of_morpholgy; i++)
    {
		cv::morphologyEx(morImage, morImage, MORPH_CLOSE, element5);
		cv::morphologyEx(morImage, morImage, MORPH_OPEN, element5);
	}
    
    return morImage;
}

void JajankenHunterApp::update()
{
    xtion->update();
    cv::Mat colorImage = xtion->getColorImage();
    cv::Mat depthImage = xtion->getDepthImage();
    
    cv::Mat morDepthImage = closeAndOpen(depthImage);
    
    //handInfo = handRectCutter->calcHandRect(depthImage);
    
    EHAND leftPersonResult = eHAND_ERROR;
	EHAND rightPersonResult = eHAND_ERROR;
	recognizer.recognizeHandByImage(morDepthImage, leftPersonResult, rightPersonResult);
    
    cv::Rect leftHandRect, rightHandRect;
	recognizer.getHandRect(leftHandRect, rightHandRect);
    
	if (leftHandRect.width > 0 && leftHandRect.height > 0 && rightHandRect.width > 0 && rightHandRect.height > 0)
    {
        cv::rectangle(colorImage, leftHandRect, cv::Scalar(255), 3);
        cv::rectangle(colorImage, rightHandRect, cv::Scalar(255), 3);
        
        kasuyaEffector[0]->setPos(cv::Vec2f(leftHandRect.x + leftHandRect.width / 2, leftHandRect.y + leftHandRect.height / 2));
        kasuyaEffector[1]->setPos(cv::Vec2f(rightHandRect.x + rightHandRect.width / 2, rightHandRect.y + rightHandRect.height / 2));
        
        kasuyaEffector[0]->setTrick(leftPersonResult);
        kasuyaEffector[1]->setTrick(rightPersonResult);
        
        kasuyaEffector[0]->start();
        kasuyaEffector[1]->start();
    }
    else
    {
        kasuyaEffector[0]->stop();
        kasuyaEffector[1]->stop();
    }
    
    kasuyaEffector[0]->update();
    kasuyaEffector[1]->update();
    
    cv::putText(colorImage, getTrickName(leftPersonResult), cv::Point(100, 50), CV_FONT_HERSHEY_PLAIN, 2, cv::Scalar(255,255,255));
    cv::putText(colorImage, getTrickName(rightPersonResult), cv::Point(300, 50), CV_FONT_HERSHEY_PLAIN, 2, cv::Scalar(255,255,255));
    
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
