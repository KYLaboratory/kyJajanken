#pragma once
#include <vector>
#include "cinder/Rand.h"
#include "cinder/BSpline.h"
#include "Janken.h"

using namespace std;
class Particle {
public:
    Particle(ci::Vec2i location, EHAND gcp, cinder::Color _color);
    void update();
    void draw();
    bool isDead();
 
protected:
    ci::Vec2f location;
    ci::Vec2f direction;
    float velocity;
    float radius;
    int life;
	EHAND GCP;
	cinder::Color color;

	vector<ci::Vec2f> pastPoints;
};