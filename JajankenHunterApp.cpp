#include "cinder/app/AppNative.h"
#include "cinder/gl/Texture.h"

#include "Xtion.h"

#include "KasuyaEffector.h"
#include "JankenRecognizer.h"
#include "AuraGenerator.h"

using namespace ci;
using namespace ci::app;
using namespace std;

#define WINDOW_HEIGHT 480
#define WINDOW_WIDTH 640

class JajankenHunterApp : public AppNative
{
    XtionRef xtion;
    gl::TextureRef texture;
    gl::TextureRef auraTexture;
    vector<KasuyaEffectorRef> kasuyaEffector;
    
    JankenRecognizer recognizer;
    
    AuraGenerator auraGenerator;
    
    bool judgementModeIsOn;
    bool tricksAreFixed;
    bool effectIsRunning;
    
    int effectorStepCount; //暫定策
    
    static string getTrickName(const EHAND& hand);
    static cv::Mat closeAndOpen(const cv::Mat& inputImage);
    static void rectangleHand(const cv::Rect& leftHandRect, const cv::Rect& rightHandRect, cv::Mat& image);
    
    void startEffector(const cv::Rect& leftHandRect, const cv::Rect& rightHandRect, const EHAND& leftPersonResult, const EHAND& rightPersonResult);
    void updateEffector();
    void stopEffector();
    
  public:
    void prepareSettings(Settings* settings);
	void setup();
    
	void mouseDown(MouseEvent event);
    void keyDown(KeyEvent event);
	
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
    
    recognizer.initializeRecognizer();
    
    kasuyaEffector.push_back(KasuyaEffector::create(cv::Vec2f(100.0f,100.0f), Color(1.0f, 1.0f, 0.0f)));
    kasuyaEffector.push_back(KasuyaEffector::create(cv::Vec2f(400.0f,100.0f), Color(0.0f, 1.0f, 1.0f)));
    
    judgementModeIsOn = false;
    tricksAreFixed = false;
    effectIsRunning = false;
    
    effectorStepCount = 0;
}

void JajankenHunterApp::mouseDown( MouseEvent event )
{
    
}

void JajankenHunterApp::keyDown(KeyEvent event)
{
    judgementModeIsOn = !judgementModeIsOn;
}

string JajankenHunterApp::getTrickName(const EHAND& hand)
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

cv::Mat JajankenHunterApp::closeAndOpen(const cv::Mat& inputImage)
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

void JajankenHunterApp::rectangleHand(const cv::Rect& leftHandRect, const cv::Rect& rightHandRect, cv::Mat& image)
{
    if (leftHandRect.width > 0 && leftHandRect.height > 0 && rightHandRect.width > 0 && rightHandRect.height > 0)
    {
        cv::rectangle(image, leftHandRect, cv::Scalar(255), 3);
        cv::rectangle(image, rightHandRect, cv::Scalar(255), 3);
    }
}

void JajankenHunterApp::startEffector(const cv::Rect& leftHandRect, const cv::Rect& rightHandRect, const EHAND& leftPersonResult, const EHAND& rightPersonResult)
{
    kasuyaEffector[0]->setPos(cv::Vec2f(leftHandRect.x + leftHandRect.width / 2, leftHandRect.y + leftHandRect.height / 2));
    kasuyaEffector[1]->setPos(cv::Vec2f(rightHandRect.x + rightHandRect.width / 2, rightHandRect.y + rightHandRect.height / 2));
    
    kasuyaEffector[0]->setTrick(leftPersonResult);
    kasuyaEffector[1]->setTrick(rightPersonResult);
    
    kasuyaEffector[0]->start();
    kasuyaEffector[1]->start();
}

void JajankenHunterApp::updateEffector()
{
    kasuyaEffector[0]->update();
    kasuyaEffector[1]->update();
}

void JajankenHunterApp::stopEffector()
{
    kasuyaEffector[0]->stop();
    kasuyaEffector[1]->stop();
}

void JajankenHunterApp::update()
{
    xtion->update();
    cv::Mat colorImage = xtion->getColorImage();
    cv::Mat depthImage = xtion->getDepthImage();
    cv::Mat morDepthImage = closeAndOpen(depthImage);
    
    if(judgementModeIsOn)
    {
        EHAND leftPersonResult = eHAND_ERROR;
        EHAND rightPersonResult = eHAND_ERROR;
        recognizer.recognizeHandByImage(morDepthImage, leftPersonResult, rightPersonResult);
        
        cv::Rect leftHandRect, rightHandRect;
        recognizer.getHandRect(leftHandRect, rightHandRect);
        
        rectangleHand(leftHandRect, rightHandRect, colorImage);
        
        if(!tricksAreFixed && leftPersonResult != eHAND_ERROR && rightPersonResult != eHAND_ERROR)
        {
            tricksAreFixed = true;
        }
        
        if(tricksAreFixed)
        {
            if(!effectIsRunning)
            {
                startEffector(leftHandRect, rightHandRect, leftPersonResult, rightPersonResult);
                effectIsRunning = true;
            }
            else
            {
                if(effectorStepCount < 20)
                {
                    updateEffector();
                    effectorStepCount++;
                }
                else
                {
                    stopEffector();
                    effectorStepCount = 0;
                    effectIsRunning = false;
                    tricksAreFixed = false;
                    judgementModeIsOn = false;
                }
            }
        }
        
        cv::putText(colorImage, getTrickName(leftPersonResult), cv::Point(100, 50), CV_FONT_HERSHEY_PLAIN, 2, cv::Scalar(255,255,255));
        cv::putText(colorImage, getTrickName(rightPersonResult), cv::Point(300, 50), CV_FONT_HERSHEY_PLAIN, 2, cv::Scalar(255,255,255));
    }
    
    cv::Mat resultImage = auraGenerator.generateAura(colorImage, morDepthImage);
    
    texture = gl::Texture::create(fromOcv(colorImage));
    
    auraTexture = gl::Texture::create(fromOcv(resultImage));
}

void JajankenHunterApp::draw()
{
	gl::clear( ColorA(0, 0, 0, 0) );
    gl::enableAlphaBlending();
    
    if( texture )
    {
        gl::draw(texture);
    }
    
    if (auraTexture) {
        gl::draw(auraTexture);
    }
    
    kasuyaEffector[0]->draw();
    kasuyaEffector[1]->draw();
    
    gl::disableAlphaBlending();
}

CINDER_APP_NATIVE( JajankenHunterApp, RendererGl )
