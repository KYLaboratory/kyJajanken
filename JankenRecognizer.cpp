#include "JankenRecognizer.h"


JankenRecognizer::JankenRecognizer(void)
{
	initializeRecognizer();
}


JankenRecognizer::~JankenRecognizer(void)
{
}


void JankenRecognizer::recognizeHandByImage(const Mat& image, EHAND& leftPersonResult, EHAND& rightPersonResult)
{
	Mat leftHand, rightHand;
	clipper.clipHands(image, leftHand, rightHand);
    
    leftPersonResult = eHAND_ERROR;
	if(leftHand.rows * leftHand.cols > 0)
    {
		//imwrite("leftHand.bmp",leftHand);
		leftPersonResult = recognizer.recognize(leftHand);
	}
    
    rightPersonResult = eHAND_ERROR;
	if(rightHand.rows * rightHand.cols > 0)
    {
		//imwrite("rightHand.bmp",rightHand);
		rightPersonResult = recognizer.recognize(rightHand);
	}
}


void JankenRecognizer::getHandRect(cv::Rect& leftHandRect, cv::Rect& rightHandRect)
{
	leftHandRect = clipper.leftHandInfo.handRect;
	rightHandRect = clipper.rightHandInfo.handRect;
}

void JankenRecognizer::initializeRecognizer()
{
	recognizer.addSample("sample/leftchoki.bmp", eHAND_SCISSORS);  //0
	recognizer.addSample("sample/leftchoki4.bmp", eHAND_SCISSORS); //1
	recognizer.addSample("sample/leftchoki2.bmp", eHAND_SCISSORS);//2
	recognizer.addSample("sample/leftchoki3.bmp", eHAND_SCISSORS);//3
    
	recognizer.addSample("sample/rightchoki.bmp", eHAND_SCISSORS);//4
	recognizer.addSample("sample/rightchoki4.bmp", eHAND_SCISSORS);//5
	recognizer.addSample("sample/rightchoki2.bmp", eHAND_SCISSORS);//6
	recognizer.addSample("sample/rightchoki3.bmp", eHAND_SCISSORS);//7

	recognizer.addSample("sample/leftgu.bmp", eHAND_ROCK);//8
	recognizer.addSample("sample/leftgu1.bmp", eHAND_ROCK);//9
	recognizer.addSample("sample/leftgu2.bmp", eHAND_ROCK);//10
	recognizer.addSample("sample/leftgu3.bmp", eHAND_ROCK);//11
    
	recognizer.addSample("sample/rightgu.bmp", eHAND_ROCK);//12
	recognizer.addSample("sample/rightgu1.bmp", eHAND_ROCK);//13
	recognizer.addSample("sample/rightgu2.bmp", eHAND_ROCK);//14
	recognizer.addSample("sample/rightgu3.bmp", eHAND_ROCK);//15

	recognizer.addSample("sample/leftpa.bmp", eHAND_PAPER);//16
	recognizer.addSample("sample/leftpa1.bmp", eHAND_PAPER);//17
	recognizer.addSample("sample/leftpa2.bmp", eHAND_PAPER);//18
	recognizer.addSample("sample/leftpa3.bmp", eHAND_PAPER);//19

	
	recognizer.addSample("sample/rightpa.bmp", eHAND_PAPER);//20
	recognizer.addSample("sample/rightpa1.bmp", eHAND_PAPER);//21
	recognizer.addSample("sample/rightpa2.bmp", eHAND_PAPER);//22
	recognizer.addSample("sample/rightpa3.bmp", eHAND_PAPER);//23
}