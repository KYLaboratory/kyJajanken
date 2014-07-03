// opencvtest.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include<vector>
#include "Labeling.h"
#include<iostream>
#include "handClip.h"


//short値画像imageにおいて、x座標がleftとrightの間にある、画素値がindexの画素の最大の高さを求める
//indexは1-255までを対象とする。その値が見つからない場合0を返す
int calcMaxHeight(cv::Mat image, int left, int right, int index, int& top, int& bottom){
	int maxHeight = 0;
	top = image.rows;//最も下のy座標を初期値とする
	bottom = 0;
	for(int x = left; x < right; x++){
		int start=0;
		int end =0;
		bool isStarted = false;
		for(int y = 0; y < image.rows; y++){
			//上から順番に見ていく
			unsigned char pixVal = image.data [y * image.step + 2 * image.channels() * x];//short画像を対象としているので2をかけている
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

// 適当な色を出す用
cv::RNG rnd(1192);
cv::Scalar randomColor()
{
    return cv::Scalar(rnd.next() & 0xFF, rnd.next() & 0xFF, rnd.next() & 0xFF);
}


//手の情報を持つクラス

HandInfo::HandInfo()
{
}

HandInfo::HandInfo(int left, int top, int right, int bottom, bool isLeftPerson)
{
	handRect = cv::Rect(left, top, right-left, bottom-top);
	if(isLeftPerson){
		personLabel = 1;
	}
	else{
		personLabel = 2;
	}
}

bool HandInfo::isLeftPersonHand(){
	if(personLabel == 1){
		return true;
	}
	else{
		return false;
	}
}


int HandInfo::getLeft()
{
	return handRect.x;
}
	
int HandInfo::getRight()
{
	return handRect.x + handRect.width;
}


bool getHandsInfo(cv::Mat binImage, HandInfo& leftInfo, HandInfo& rightInfo)
{
	vector<HandInfo> leftHands;
	vector<HandInfo> rightHands;

	cv::Mat labelImage(binImage.size(), CV_16SC1);
 
    // ラベリングを実施 ２値化した画像に対して実行する。
    LabelingBS  labeling;
    labeling.Exec(binImage.data, (short *)labelImage.data, binImage.cols, binImage.rows, true, 2000);  //大きさ2,000以下のラベルは切り捨て
     

	for(int i=0; i< labeling.GetNumOfRegions();i++){//すべてのリージョンに対して実施

		Labeling<unsigned char,short>::RegionInfo *labelInfo = labeling.GetResultRegionInfo(i);

		float gx, gy;
		labelInfo->GetCenterOfGravity(gx, gy);

		int top1, bottom1, left1, right1;
		if( gx > binImage.cols / 2){//重心が中央より右にあれば右の人の手として処理
			int top, left;
			labelInfo->GetMin(left, top);
			//領域の左から６０pixのところまでを借りの手の領域として、その高さを求める
			int height = calcMaxHeight(labelImage, left, min(left+60,binImage.cols), i+1, top1, bottom1);
			left1 = left;
			right1=min((int)(left+height * 1.5), binImage.cols);//高さの２倍を手の横幅とする
			calcMaxHeight(labelImage, left1, right1, i+1, top1, bottom1);//その領域で上下の最大値を求める
			rightHands.push_back(HandInfo(left1, top1, right1, bottom1, false));
		}
		
		else{//重心が中央より左にあれば左の人の手として処理
			int right, bottom;
			labelInfo->GetMax(right, bottom);
			int height =calcMaxHeight(labelImage, max(right-60,0), right, i+1, top1,bottom1);
			left1 = max((int)(right - 1.5 * height), 0);
			right1 = right;
			calcMaxHeight(labelImage, left1, right1, i+1, top1, bottom1);
			leftHands.push_back(HandInfo(left1,top1, right1, bottom1, false));
		}
	}

	bool isLeftHandsExist = false;
    bool isRightHandsExist = false;

	//左手候補が複数ある場合の選別
	if(leftHands.size() > 0){
		leftInfo = leftHands[0];
		isLeftHandsExist = true;
		//より右端が右にある手を残す
		for(int i=1; i < leftHands.size(); i++){
			if(leftHands[i].getRight() > leftInfo.getRight()){
				leftInfo = leftHands[i];
			}
		}
	}
	
	//右手候補が複数ある場合の選別
	if(rightHands.size() > 0){
		rightInfo = rightHands[0];
		isRightHandsExist = true;
		//より左端が左にある手を残す
		for(int i=1; i < rightHands.size(); i++){
			if(rightHands[i].getLeft() < rightHands[i].getRight()){
				rightInfo = rightHands[i];
			}
		}
	}

	return isLeftHandsExist && isRightHandsExist;
}
