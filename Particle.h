#pragma once
#include "cinder/Vector.h"
#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include <vector>
using namespace std;
class Particle {
public:
    Particle(ci::Vec2i location, int gcp, cinder::Color _color);
    void update();
    void draw();
    bool isDead();
 
protected:
    ci::Vec2f location;
    ci::Vec2f direction;
    float velocity;
    float radius;
    int life;
	int GCP;
	cinder::Color color;

	vector<ci::Vec2f> pastPoints;
};