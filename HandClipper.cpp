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
    // ���x�����O�����{ �Q�l�������摜�ɑ΂��Ď��s����B
    LabelingBS  labeling;
    labeling.Exec(subImage.data, (short *)labelImage.data, subImage.cols, subImage.rows, true, 0);
    Labeling<unsigned char,short>::RegionInfo *labelInfo = labeling.GetResultRegionInfo(0);
    
    labelInfo->GetMax(right, bottom);
    labelInfo->GetMin(left, top);
    
	return bottom - top;
}

//short�l�摜image�ɂ����āAx���W��left��right�̊Ԃɂ���A��f�l��index�̉�f�̍ő�̍��������߂�
//index��1-255�܂ł�ΏۂƂ���B���̒l��������Ȃ��ꍇ0��Ԃ�
int HandClipper::calcMaxHeight(const Mat& image, int left, int right, int index, int& top, int& bottom) const
{
	int depth = 1;
	if(image.depth() == CV_16S){
		depth = 2;
	}
	int maxHeight = 0;
	top = image.rows;//�ł�����y���W�������l�Ƃ���
	bottom = 0;
	for(int x = left; x < right; x++){
		int start=0;
		int end =0;
		bool isStarted = false;
		for(int y = 0; y < image.rows; y++){
			//�ォ�珇�ԂɌ��Ă���
			unsigned char pixVal = image.data [y * image.step + depth * image.channels() * x];//short�摜��ΏۂƂ��Ă���̂�2�������Ă���
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
        
		if(isStarted){//index�Ɠ������s�N�Z���������maxHeight,top,bottom���X�V
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



//��̉摜�̊p�x���ŏ����@�Ōv�Z����
//return �����̕����x�N�g��vec4f[0],[1]�ƒʉߓ_vec4f[2],[3]
Vec4f HandClipper::calcHandAngle(const Mat& image, const cv::Rect& handRect) const
{
	std::vector<Point2f> points;//�����s�N�Z�������ׂĊi�[����
	Vec4f line;
	for(int y = handRect.y; y < handRect.y + handRect.height; y++){
		for(int x = handRect.x; x <handRect.x + handRect.width; x++){
			if(image.data[y * image.step + image.channels() *x] != 0){
				Point2f point(x,y);
				points.push_back(point);
			}
		}
	}
	fitLine(points,line,CV_DIST_L2,0,0.01,0.01);//��A�������v�Z
	return line;
}


//���̐l�ƉE�̐l�̎�̋�`�̈��T������
//return �����̐l�̎肪�Ƃ�Ă����true����ȊO��false
bool HandClipper::getHandsInfo(const Mat& binImage, HandInfo& leftInfo, HandInfo& rightInfo, Mat& labelImage) const
{
    // ���x�����O�����{ �Q�l�������摜�ɑ΂��Ď��s����B
	//imwrite("binimg.bmp",binImage);
	labelImage = Mat(binImage.size(), CV_16SC1);
    LabelingBS labeling;
    labeling.Exec(binImage.data, (short *)labelImage.data, binImage.cols, binImage.rows, true, 0);
    
    const int seekWidth = 110;
	const double aspectRatio = 2.0;
    vector<HandInfo> leftHands;
	vector<HandInfo> rightHands;
	for(int i = 0; i < labeling.GetNumOfRegions();i++)
    {//���ׂẴ��[�W�����ɑ΂��Ď��{
		Labeling<unsigned char,short>::RegionInfo *labelInfo = labeling.GetResultRegionInfo(i);
		if(labelInfo->GetNumOfPixels() < 10000)
        {
			continue; //�傫��2,000�ȉ��̃��x���͐؂�̂�
		}
        
		float gx, gy;
		labelInfo->GetCenterOfGravity(gx, gy);
        
		if(gx > binImage.cols / 2)
        {//�d�S���������E�ɂ���ΉE�̐l�̎�Ƃ��ď�
            int labelLeft, labelTop;
			labelInfo->GetMin(labelLeft, labelTop);
			//�̈�̍�����110pix�̂Ƃ���܂ł��؂�̎�̗̈�Ƃ��āA���̍��������߂�
            const int labelRight = min(labelLeft + seekWidth, binImage.cols);
            int handTop, handBottom;
			const int handHeight = calcMaxHeight(labelImage, labelLeft, labelRight, i + 1, handTop, handBottom);
			if(handHeight == 0)
            {
				continue;
			}
            const int handLeft = labelLeft;
			const int handRight = min((int)(labelLeft + handHeight * aspectRatio), binImage.cols);//�����̂Q�{����̉����Ƃ���
			calcMaxHeightEx(labelImage, handLeft, handRight, i + 1, handTop, handBottom);//���̗̈�ŏ㉺�̍ő�l�����߂�
			rightHands.push_back(HandInfo(handLeft, handTop, handRight, handBottom, i + 1, RIGHT_PERSON));//index�̒l�̓��x���̏���i + 1
		}
		else
        {//�d�S��������荶�ɂ���΍��̐l�̎�Ƃ��ď���
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
			leftHands.push_back(HandInfo(handLeft, handTop, handRight, handBottom, i + 1, LEFT_PERSON));//index�̒l�̓��x���̏���i + 1
		}
	}
    
	bool isLeftHandsExist = false;
	//�����₪��������ꍇ�̑I�
	if(leftHands.size() > 0)
    {
		leftInfo = leftHands[0];
		isLeftHandsExist = true;
		//���E�[���E�ɂ������c��
		for(int i = 1; i < leftHands.size(); i++)
        {
			if(leftHands[i].getRight() > leftInfo.getRight())
            {
				leftInfo = leftHands[i];
			}
		}
	}
	
    bool isRightHandsExist = false;
	//�E���₪��������ꍇ�̑I�
	if(rightHands.size() > 0)
    {
		rightInfo = rightHands[0];
		isRightHandsExist = true;
		//��荶�[�����ɂ������c��
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

//�摜���E�[���璲�ׂāA�ŏ��ɔ����s�N�Z�����o�Ă���x���W��Ԃ�
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

//�����]���l�����Đ؂�o��
Mat HandClipper::clipHand(const Mat& binimg, const HandInfo& hand) const
{
	const Vec4f line = calcHandAngle(binimg, hand.handRect);
	const double angle =  atan2(line[1],line[0])* 180.0 / 3.1415;
	const Mat imgR = rotateEx(binimg, angle, hand.handRect);
	if(!hand.isLeftPersonHand())
    {
		flip(imgR, imgR, 1);//�E�̐l�̎�̏ꍇ���]������
	}
    
    const double aspectRatio = 1.5;
	int top, bottom;
	const int height = calcMaxHeightEx(imgR, 0, imgR.cols, 255, top, bottom);//y�����̍ő卂�����擾
	const int right = getRightEdge(imgR);
	const int left = max(0.0, right - height * aspectRatio);
	const Mat imgRC = imgR(cv::Rect(cv::Point(left,top), cv::Point(right,bottom)));//�ꎞ�؂�o��
	calcMaxHeight(imgRC, 0, imgRC.cols, 255, top, bottom);//�X�ɗ]���ȏ㉺��؂�
	return imgRC(cv::Rect(cv::Point(0,top), cv::Point(imgRC.cols, bottom)));
}

