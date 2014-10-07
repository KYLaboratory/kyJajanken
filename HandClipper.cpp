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
	threshold(image, binimg, 180, 255, THRESH_BINARY);
    
	HandInfo leftHand;
	HandInfo rightHand;
	Mat labelImage;

	if(getHandsInfo(binimg, leftHand, rightHand, labelImage)){
		if(leftHand.isFound){
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
		if(rightHand.isFound){
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
	const Mat subImage(cv::Size(right - left, image.rows), CV_8UC1, Scalar(0));
    
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
    
    // ラベリングを実施 ２値化した画像に対して実行する。
    LabelingBS  labeling;
    labeling.Exec(subImage.data, (short *)labelImage.data, subImage.cols, subImage.rows, true, 0);
    
    if(labeling.GetNumOfResultRegions() > 0)
    {
        Labeling<unsigned char,short>::RegionInfo *labelInfo = labeling.GetResultRegionInfo(0);
    
        labelInfo->GetMax(right, bottom);
        labelInfo->GetMin(left, top);
    }
    else
    {
        top = 0;
        bottom = image.rows - 1;
    }
    
	return bottom - top;
}

//short値画像imageにおいて、x座標がleftとrightの間にある、画素値がindexの画素の最大の高さを求める
//indexは1-255までを対象とする。その値が見つからない場合0を返す
int HandClipper::calcMaxHeight(const Mat& image, int left, int right, int index, int& top, int& bottom) const
{
	int depth = 1;
	if(image.depth() == CV_16S){
		depth = 2;
	}
	int maxHeight = 0;
	top = image.rows;//最も下のy座標を初期値とする
	bottom = 0;
	for(int x = left; x < right; x++){
		int start=0;
		int end =0;
		bool isStarted = false;
		for(int y = 0; y < image.rows; y++){
			//上から順番に見ていく
			unsigned char pixVal = image.data [y * image.step + depth * image.channels() * x];//short画像を対象としているので2をかけている
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
        
		if(isStarted){//indexと等しいピクセルがあればmaxHeight,top,bottomを更新
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



//手の画像の角度を最小二乗法で計算する
//return 直線の方向ベクトルvec4f[0],[1]と通過点vec4f[2],[3]
Vec4f HandClipper::calcHandAngle(const Mat& image, const cv::Rect& handRect) const
{
	std::vector<Point2f> points;//白いピクセルをすべて格納する
	Vec4f line;
	for(int y = handRect.y; y < handRect.y + handRect.height; y++){
		for(int x = handRect.x; x <handRect.x + handRect.width; x++){
			if(image.data[y * image.step + image.channels() *x] != 0){
				Point2f point(x,y);
				points.push_back(point);
			}
		}
	}
	fitLine(points,line,CV_DIST_L2,0,0.01,0.01);//回帰直線を計算
	return line;
}


//左の人と右の人の手の矩形領域を探索する
//return 両方の人の手がとれていればtrueそれ以外はfalse
bool HandClipper::getHandsInfo(const Mat& binImage, HandInfo& leftInfo, HandInfo& rightInfo, Mat& labelImage) const
{
    // ラベリングを実施 ２値化した画像に対して実行する。
	//imwrite("binimg.bmp",binImage);
	labelImage = Mat(binImage.size(), CV_16SC1);
    LabelingBS labeling;
    labeling.Exec(binImage.data, (short *)labelImage.data, binImage.cols, binImage.rows, true, 0);
    
    const int seekWidth = 110;
	const double aspectRatio = 2.0;
    vector<HandInfo> leftHands;
	vector<HandInfo> rightHands;
	for(int i = 0; i < labeling.GetNumOfRegions();i++)
    {//すべてのリージョンに対して実施
		Labeling<unsigned char,short>::RegionInfo *labelInfo = labeling.GetResultRegionInfo(i);
		if(labelInfo->GetNumOfPixels() < 10000)
        {
			continue; //大きさ2,000以下のラベルは切り捨て
		}
        
		float gx, gy;
		labelInfo->GetCenterOfGravity(gx, gy);
        
		if(gx > binImage.cols / 2)
        {//重心が中央より右にあれば右の人の手として処
            int labelLeft, labelTop;
			labelInfo->GetMin(labelLeft, labelTop);
			//領域の左から110pixのところまでを借りの手の領域として、その高さを求める
            const int labelRight = min(labelLeft + seekWidth, binImage.cols);
            int handTop, handBottom;
			const int handHeight = calcMaxHeight(labelImage, labelLeft, labelRight, i + 1, handTop, handBottom);
			if(handHeight == 0)
            {
				continue;
			}
            const int handLeft = labelLeft;
			const int handRight = min((int)(labelLeft + handHeight * aspectRatio), binImage.cols);//高さの２倍を手の横幅とする
			calcMaxHeightEx(labelImage, handLeft, handRight, i + 1, handTop, handBottom);//その領域で上下の最大値を求める
			rightHands.push_back(HandInfo(handLeft, handTop, handRight, handBottom, i + 1, RIGHT_PERSON));//indexの値はラベルの順番i + 1
		}
		else
        {//重心が中央より左にあれば左の人の手として処理
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
			leftHands.push_back(HandInfo(handLeft, handTop, handRight, handBottom, i + 1, LEFT_PERSON));//indexの値はラベルの順番i + 1
		}
	}
    
	bool isLeftHandsExist = false;
	//左手候補が複数ある場合の選・	
	if(leftHands.size() > 0)
    {
		leftInfo = leftHands[0];
		leftInfo.isFound = true;
		isLeftHandsExist =true;
		//より右端が右にある手を残す
		for(int i = 1; i < leftHands.size(); i++)
        {
			if(leftHands[i].getRight() > leftInfo.getRight())
            {
				leftInfo = leftHands[i];
				leftInfo.isFound = true;
			}
		}
	}
	
    bool isRightHandsExist = false;
	//右手候補が複数ある場合の選・	
	if(rightHands.size() > 0)
    {
		rightInfo = rightHands[0];
		rightInfo.isFound = true;
		isRightHandsExist = true;
		//より左端が左にある手を残す
		for(int i = 1; i < rightHands.size(); i++)
        {
			if(rightHands[i].getLeft() < rightInfo.getLeft())
            {
				rightInfo = rightHands[i];
				rightInfo.isFound = true;
			}
		}
	}
    
	return isLeftHandsExist && isRightHandsExist;
}

//画像を右端から調べて、最初に白いピクセルが出てくるx座標を返す
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

//手を回転を考慮して切り出す
Mat HandClipper::clipHand(const Mat& binimg, const HandInfo& hand) const
{
	const Vec4f line = calcHandAngle(binimg, hand.handRect);
	const double angle =  atan2(line[1],line[0])* 180.0 / 3.1415;
    const Mat imgR = rotateEx(binimg, angle, hand.handRect);
	if(!hand.isLeftPersonHand())
    {
		flip(imgR, imgR, 1);//右の人の手の場合反転させる
	}
    
    const double aspectRatio = 1.5;
	int top, bottom;
	const int height = calcMaxHeightEx(imgR, 0, imgR.cols, 255, top, bottom);//y方向の最大高さを取得
	const int right = getRightEdge(imgR);
	const int left = max(0.0, right - height * aspectRatio);
	const Mat imgRC = imgR(cv::Rect(cv::Point(left,top), cv::Point(right,bottom)));//一時切り出し
	calcMaxHeight(imgRC, 0, imgRC.cols, 255, top, bottom);//更に余分な上下を切る
	return imgRC(cv::Rect(cv::Point(0,top), cv::Point(imgRC.cols, bottom)));
}

