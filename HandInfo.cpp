//
//  HandInfo.cpp
//  JajankenHunter
//
//  Created by akihito on 2014/08/13.
//
//

#include "HandInfo.h"

HandInfo::HandInfo()
{
    isFound = false;
}

HandInfo::HandInfo(int left, int top, int right, int bottom,int _index, bool isLeftPerson)
{
    isFound = true;
    handRect = cv::Rect(left, top, right-left, bottom-top);
    index = _index;
    if(isLeftPerson){
        personLabel = 1;
    }
    else{
        personLabel = 2;
    }
}

HandInfo::~HandInfo()
{
    
}

bool HandInfo::isLeftPersonHand() const
{
    if(personLabel == 1){
        return true;
    }
    else{
        return false;
    }
}

int HandInfo::getLeft() const
{
    return handRect.x;
}

int HandInfo::getRight() const
{
    return handRect.x + handRect.width;
}
