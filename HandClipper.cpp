#include "HandClipper.h"


HandClipper::HandClipper(void)
{
}


HandClipper::~HandClipper(void)
{
}

void HandClipper::clipHands(const Mat& image, Mat& leftPersonImage, Mat& rightPersonImage)
{
	Mat binimg;
	threshold(image, binimg, 200, 255, THRESH_BINARY);
    
	HandInfo leftHand;
	HandInfo rightHand;
	Mat labelImage;
	getHandsInfo(binimg, leftHand, rightHand, labelImage);
	
	if(leftHand.isFound)
    {
        Mat labelarea;
		compare(labelImage, leftHand.index, labelarea, CV_CMP_EQ);
        
        Mat clipImage(binimg.size(), CV_8UC1, Scalar(0));
        Mat color(binimg.size(), CV_8UC1, Scalar(255));
        color.copyTo(clipImage, labelarea);
        
		//Vec4f line = calcHandAngle(clipImage, leftHand.handRect);
		//double angle =  atan2(line[1],line[0])* 180.0 / 3.1415; //??
        
		leftPersonImage = clipHand(clipImage, leftHand);
		//clipImage = Scalar(0);
	}
    
	if(rightHand.isFound)
    {
        Mat labelarea;
		compare(labelImage, rightHand.index, labelarea, CV_CMP_EQ);
        
        Mat clipImage(binimg.size(), CV_8UC1, Scalar(0));
        Mat color(binimg.size(), CV_8UC1, Scalar(255));
        color.copyTo(clipImage, labelarea);
        
		Vec4f line = calcHandAngle(binimg, rightHand.handRect);
		double angle =  atan2(line[1],line[0])* 180.0 / 3.1415;
		Mat outimgR2 = rotateEx(clipImage, angle, rightHand.handRect);//??
        
		rightPersonImage = clipHand(clipImage, rightHand);
	}
    
	leftHandInfo = leftHand;
	rightHandInfo = rightHand;
}


Mat HandClipper::rotateEx(const Mat& src, const double angle, const cv::Rect& rect) const
{
    Mat dst;
	Point2f pt(rect.x,rect.y);
	double rad = 3.14*angle/180;
	double newWidth = cos(rad)*rect.width + sin(abs(rad))*rect.height;
	double newHeight = sin(abs(rad))*rect.width + cos(rad)*rect.height;
    
	vector<Point2f> srcVec;
	vector<Point2f> dstVec;
    
	srcVec.push_back(Point2f(rect.x, rect.y));
	srcVec.push_back(Point2f(rect.x + rect.width, rect.y));
	srcVec.push_back(Point2f(rect.x, rect.y+ rect.height));
    
	if(angle >0){
		dstVec.push_back(Point2f(0, rect.width*sin(rad)));
		dstVec.push_back(Point2f(rect.width*cos(rad)));
		dstVec.push_back(Point2f(rect.height*sin(rad),newHeight));
	}
	else{
		dstVec.push_back(Point2f(rect.height*sin(-rad), 0));
		dstVec.push_back(Point2f(newWidth, rect.width*sin(-rad)));
		dstVec.push_back(Point2f(0, rect.height*cos(-rad)));
	}
	Mat r = getAffineTransform(srcVec,dstVec);
    
	warpAffine(src, dst, r, cv::Size(newWidth, newHeight));
    return dst;
}

int HandClipper::calcMaxHeightEx(const Mat& image, int left, int right, int index, int& top, int& bottom) const
{
    const int depth = image.depth() == CV_16S ? 2 : 1;
	Mat subImage(cv::Size(right - left, image.rows), CV_8UC1, Scalar(0));
	for(int x = left; x < right; x++)
    {
		for(int y = 0; y < image.rows; y++)
        {
			const unsigned char pixVal = image.data[y * image.step + depth * image.channels() * x];
			if(pixVal == index)
            {
				subImage.data[y * subImage.step + subImage.channels() * (x - left)] = 255;
			}
		}
	}
    
	Mat labelImage(subImage.size(), CV_16SC1);
    // ƒ‰ƒxƒŠƒ“ƒO‚ğÀ{ ‚Q’l‰»‚µ‚½‰æ‘œ‚É‘Î‚µ‚ÄÀs‚·‚éB
    LabelingBS  labeling;
    labeling.Exec(subImage.data, (short *)labelImage.data, subImage.cols, subImage.rows, true, 0);
    Labeling<unsigned char,short>::RegionInfo *labelInfo = labeling.GetResultRegionInfo(0);
    
    labelInfo->GetMax(right, bottom);
    labelInfo->GetMin(left, top);
    
	return bottom - top;
}

//short’l‰æ‘œimage‚É‚¨‚¢‚ÄAxÀ•W‚ªleft‚Æright‚ÌŠÔ‚É‚ ‚éA‰æ‘f’l‚ªindex‚Ì‰æ‘f‚ÌÅ‘å‚Ì‚‚³‚ğ‹‚ß‚é
//index‚Í1-255‚Ü‚Å‚ğ‘ÎÛ‚Æ‚·‚éB‚»‚Ì’l‚ªŒ©‚Â‚©‚ç‚È‚¢ê‡0‚ğ•Ô‚·
int HandClipper::calcMaxHeight(const Mat& image, int left, int right, int index, int& top, int& bottom) const
{
	int depth = 1;
	if(image.depth() == CV_16S){
		depth = 2;
	}
	int maxHeight = 0;
	top = image.rows;//Å‚à‰º‚ÌyÀ•W‚ğ‰Šú’l‚Æ‚·‚é
	bottom = 0;
	for(int x = left; x < right; x++){
		int start=0;
		int end =0;
		bool isStarted = false;
		for(int y = 0; y < image.rows; y++){
			//ã‚©‚ç‡”Ô‚ÉŒ©‚Ä‚¢‚­
			unsigned char pixVal = image.data [y * image.step + depth * image.channels() * x];//short‰æ‘œ‚ğ‘ÎÛ‚Æ‚µ‚Ä‚¢‚é‚Ì‚Å2‚ğ‚©‚¯‚Ä‚¢‚é
			if(pixVal == index){
				if(!isStarted){
					start = y;
					end = y;
				    isStarted = true;
				}
				else{
					end = y;
				}
			}
		}
        
		if(isStarted){//index‚Æ“™‚µ‚¢ƒsƒNƒZƒ‹‚ª‚ ‚ê‚ÎmaxHeight,top,bottom‚ğXV
			int height = end - start +1;
		    if(maxHeight < height){
			    maxHeight = height;
		    }
			if(top > start){
				top = start;
			}
			if(bottom < end){
				bottom = end;
			}
		}
	}
	return maxHeight;
}



//è‚Ì‰æ‘œ‚ÌŠp“x‚ğÅ¬“ñæ–@‚ÅŒvZ‚·‚é
//return ’¼ü‚Ì•ûŒüƒxƒNƒgƒ‹vec4f[0],[1]‚Æ’Ê‰ß“_vec4f[2],[3]
Vec4f HandClipper::calcHandAngle(const Mat& image, const cv::Rect& handRect) const
{
	std::vector<Point2f> points;//”’‚¢ƒsƒNƒZƒ‹‚ğ‚·‚×‚ÄŠi”[‚·‚é
	Vec4f line;
	for(int y = handRect.y; y < handRect.y + handRect.height; y++){
		for(int x = handRect.x; x <handRect.x + handRect.width; x++){
			if(image.data[y * image.step + image.channels() *x] != 0){
				Point2f point(x,y);
				points.push_back(point);
			}
		}
	}
	fitLine(points,line,CV_DIST_L2,0,0.01,0.01);//‰ñ‹A’¼ü‚ğŒvZ
	return line;
}


//¶‚Ìl‚Æ‰E‚Ìl‚Ìè‚Ì‹éŒ`—Ìˆæ‚ğ’Tõ‚·‚é
//return —¼•û‚Ìl‚Ìè‚ª‚Æ‚ê‚Ä‚¢‚ê‚Îtrue‚»‚êˆÈŠO‚Ífalse
bool HandClipper::getHandsInfo(const Mat& binImage, HandInfo& leftInfo, HandInfo& rightInfo, Mat& labelImage) const
{
    // ƒ‰ƒxƒŠƒ“ƒO‚ğÀ{ ‚Q’l‰»‚µ‚½‰æ‘œ‚É‘Î‚µ‚ÄÀs‚·‚éB
	//imwrite("binimg.bmp",binImage);
	labelImage = Mat(binImage.size(), CV_16SC1);
    LabelingBS labeling;
    labeling.Exec(binImage.data, (short *)labelImage.data, binImage.cols, binImage.rows, true, 0);
    
    const int seekWidth = 110;
	const double aspectRatio = 2.0;
    vector<HandInfo> leftHands;
	vector<HandInfo> rightHands;
	for(int i = 0; i < labeling.GetNumOfRegions();i++)
    {//‚·‚×‚Ä‚ÌƒŠ[ƒWƒ‡ƒ“‚É‘Î‚µ‚ÄÀ{
		Labeling<unsigned char,short>::RegionInfo *labelInfo = labeling.GetResultRegionInfo(i);
		if(labelInfo->GetNumOfPixels() < 10000)
        {
			continue; //‘å‚«‚³2,000ˆÈ‰º‚Ìƒ‰ƒxƒ‹‚ÍØ‚èÌ‚Ä
		}
        
		float gx, gy;
		labelInfo->GetCenterOfGravity(gx, gy);
        
		if(gx > binImage.cols / 2)
        {//dS‚ª’†‰›‚æ‚è‰E‚É‚ ‚ê‚Î‰E‚Ìl‚Ìè‚Æ‚µ‚Äˆ
            int labelLeft, labelTop;
			labelInfo->GetMin(labelLeft, labelTop);
			//—Ìˆæ‚Ì¶‚©‚ç110pix‚Ì‚Æ‚±‚ë‚Ü‚Å‚ğØ‚è‚Ìè‚Ì—Ìˆæ‚Æ‚µ‚ÄA‚»‚Ì‚‚³‚ğ‹‚ß‚é
            const int labelRight = min(labelLeft + seekWidth, binImage.cols);
            int handTop, handBottom;
			const int handHeight = calcMaxHeight(labelImage, labelLeft, labelRight, i + 1, handTop, handBottom);
			if(handHeight == 0)
            {
				continue;
			}
            const int handLeft = labelLeft;
			const int handRight = min((int)(labelLeft + handHeight * aspectRatio), binImage.cols);//‚‚³‚Ì‚Q”{‚ğè‚Ì‰¡•‚Æ‚·‚é
			calcMaxHeightEx(labelImage, handLeft, handRight, i + 1, handTop, handBottom);//‚»‚Ì—Ìˆæ‚Åã‰º‚ÌÅ‘å’l‚ğ‹‚ß‚é
			rightHands.push_back(HandInfo(handLeft, handTop, handRight, handBottom, i + 1, RIGHT_PERSON));//index‚Ì’l‚Íƒ‰ƒxƒ‹‚Ì‡”Ôi + 1
		}
		else
        {//dS‚ª’†‰›‚æ‚è¶‚É‚ ‚ê‚Î¶‚Ìl‚Ìè‚Æ‚µ‚Äˆ—
            int labelRight, labelBottom;
			labelInfo->GetMax(labelRight, labelBottom);
            const int labelLeft = max(labelRight - seekWidth, 0);
            int handTop, handBottom;
			const int handHeight = calcMaxHeight(labelImage, labelLeft, labelRight, i + 1, handTop,handBottom);
			if(handHeight == 0)
            {
				continue;
			}
            const int handRight = labelRight;
			const int handLeft = max((int)(handRight - handHeight * aspectRatio), 0);
			calcMaxHeightEx(labelImage, handLeft, handRight, i+1, handTop, handBottom);
			leftHands.push_back(HandInfo(handLeft, handTop, handRight, handBottom, i + 1, LEFT_PERSON));//index‚Ì’l‚Íƒ‰ƒxƒ‹‚Ì‡”Ôi + 1
		}
	}
    
	bool isLeftHandsExist = false;
	//¶èŒó•â‚ª•¡”‚ ‚éê‡‚Ì‘I•
	if(leftHands.size() > 0)
    {
		leftInfo = leftHands[0];
		isLeftHandsExist = true;
		//‚æ‚è‰E’[‚ª‰E‚É‚ ‚éè‚ğc‚·
		for(int i = 1; i < leftHands.size(); i++)
        {
			if(leftHands[i].getRight() > leftInfo.getRight())
            {
				leftInfo = leftHands[i];
			}
		}
	}
	
    bool isRightHandsExist = false;
	//‰EèŒó•â‚ª•¡”‚ ‚éê‡‚Ì‘I•
	if(rightHands.size() > 0)
    {
		rightInfo = rightHands[0];
		isRightHandsExist = true;
		//‚æ‚è¶’[‚ª¶‚É‚ ‚éè‚ğc‚·
		for(int i = 1; i < rightHands.size(); i++)
        {
			if(rightHands[i].getLeft() < rightInfo.getLeft())
            {
				rightInfo = rightHands[i];
			}
		}
	}
    
	return isLeftHandsExist && isRightHandsExist;
}

//‰æ‘œ‚ğ‰E’[‚©‚ç’²‚×‚ÄAÅ‰‚É”’‚¢ƒsƒNƒZƒ‹‚ªo‚Ä‚­‚éxÀ•W‚ğ•Ô‚·
int HandClipper::getRightEdge(const Mat& binimg) const
{
	for(int x = binimg.cols - 1; x > 0; x--)
    {
		for(int y = 0; y < binimg.rows; y++)
        {
			unsigned char pixVal = binimg.data[y * binimg.step + x];
			if(pixVal != 0)
            {
				return x;
			}
		}
	}
    return 0; //toriaezu tekitou (by akihito)
}

//è‚ğ‰ñ“]‚ğl—¶‚µ‚ÄØ‚èo‚·
Mat HandClipper::clipHand(const Mat& binimg, const HandInfo& hand) const
{
	const Vec4f line = calcHandAngle(binimg, hand.handRect);
	const double angle =  atan2(line[1],line[0])* 180.0 / 3.1415;
	const Mat imgR = rotateEx(binimg, angle, hand.handRect);
	if(!hand.isLeftPersonHand())
    {
		flip(imgR, imgR, 1);//‰E‚Ìl‚Ìè‚Ìê‡”½“]‚³‚¹‚é
	}
    
    const double aspectRatio = 1.5;
	int top, bottom;
	const int height = calcMaxHeightEx(imgR, 0, imgR.cols, 255, top, bottom);//y•ûŒü‚ÌÅ‘å‚‚³‚ğæ“¾
	const int right = getRightEdge(imgR);
	const int left = max(0.0, right - height * aspectRatio);
	const Mat imgRC = imgR(cv::Rect(cv::Point(left,top), cv::Point(right,bottom)));//ˆêØ‚èo‚µ
	calcMaxHeight(imgRC, 0, imgRC.cols, 255, top, bottom);//X‚É—]•ª‚Èã‰º‚ğØ‚é
	return imgRC(cv::Rect(cv::Point(0,top), cv::Point(imgRC.cols, bottom)));
}

