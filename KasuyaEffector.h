//
//  KasuyaEffector.h
//  JajankenHunter
//
//  Created by akihito on 2014/06/22.
//
//

#ifndef __JajankenHunter__KasuyaEffector__
#define __JajankenHunter__KasuyaEffector__

#include <iostream>
#include "Particle.h"
#include "cinder/Rand.h"
#include "CinderOpenCV.h"

using namespace cv;

class KasuyaEffector;
typedef std::shared_ptr<KasuyaEffector> KasuyaEffectorRef;

class KasuyaEffector
{
    list<Particle> particles;
    bool isEmitting;
    cv::Vec2i startPos;
	int count;
	int GCP;
    cv::Vec2f beamPos;
    ci::Color color;
    
public:
    KasuyaEffector(const cv::Vec2f initPos, const ci::Color initColor);
    ~KasuyaEffector();
    
    static KasuyaEffectorRef create(const cv::Vec2f initPos, const ci::Color initColor);
    
    void setTrick(const int trick);
    void setPos(const Vec2f pos);
    void start();
    void stop();
    void update();
    void draw();
};


#endif /* defined(__JajankenHunter__KasuyaEffector__) */
