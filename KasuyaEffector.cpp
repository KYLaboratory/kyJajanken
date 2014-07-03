//
//  KasuyaEffector.cpp
//  JajankenHunter
//
//  Created by akihito on 2014/06/22.
//
//

#include "KasuyaEffector.h"

KasuyaEffector::KasuyaEffector(const Vec2f initPos, const ci::Color initColor)
:beamPos(initPos), color(initColor), count(0), GCP(1), isEmitting(false)
{
}

KasuyaEffector::~KasuyaEffector()
{
}

KasuyaEffectorRef KasuyaEffector::create(const Vec2f initPos, const ci::Color initColor)
{
    return KasuyaEffectorRef(new KasuyaEffector(initPos, initColor));
}

void KasuyaEffector::setTrick(const int trick)
{
    GCP = trick;
}

void KasuyaEffector::setPos(const Vec2f pos)
{
    startPos = pos;
}

void KasuyaEffector::start()
{
    isEmitting = true;
}

void KasuyaEffector::stop()
{
    isEmitting = false;
}

void KasuyaEffector::update()
{
    count++;
	for (list<Particle>::iterator iter = particles.begin();
         iter != particles.end();) {
        iter->update();
        if (iter->isDead())
            iter = particles.erase(iter);
        else
            iter++;
    }
    
	if (isEmitting) {
        for (int i = 0; i < 120*(1 + 0.5*sin(count*2*3.14/(30))); i++){
			particles.push_back(Particle(ci::fromOcv(startPos) + 50 * ci::Rand::randVec2f(), GCP, color));
		}
    }
}

void KasuyaEffector::draw()
{
	if(isEmitting)
    {
        for (list<Particle>::iterator iter = particles.begin();
         iter != particles.end();
         iter++)
        iter->draw();
    }
}
