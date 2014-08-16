//
//  HandInfo.h
//  JajankenHunter
//
//  Created by akihito on 2014/08/13.
//
//

#ifndef __JajankenHunter__HandInfo__
#define __JajankenHunter__HandInfo__

#include "CinderOpenCV.h"

//éËÇÃèÓïÒÇéùÇ¬ÉNÉâÉX
class HandInfo{
public:
    cv::Rect handRect;
	int personLabel;
	bool isFound;
	int index;
    
	HandInfo();
	HandInfo(int left, int top, int right, int bottom,int _index, bool isLeftPerson);
    ~HandInfo();
    
	bool isLeftPersonHand() const;
	int getLeft() const;
	int getRight() const;
};

#endif /* defined(__JajankenHunter__HandInfo__) */
