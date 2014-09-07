#include "Particle.h"

using namespace ci;

Particle::Particle(Vec2i _location,int gcp, cinder::ColorA _color)
{
	GCP = gcp;
    
    location = _location;
    direction = Rand::randVec2f();
    velocity = Rand::randFloat(8.0f);
    radius = Rand::randFloat(8.0f);
    life = Rand::randInt(50, 250);
	color = _color;
    
}

void Particle::update()
{
    location += direction * velocity;
	direction += 0.05 * Rand::randVec2f();
    
	pastPoints.push_back(location);
	if(pastPoints.size() > 12)
    {
		pastPoints.erase(pastPoints.begin());
	}
    
    switch (GCP)
    {
        case 1:
            life -= 5;
            break;
        case 2:
            direction += Vec2f(-1.0f, 0.0f);
            life -= 10;
            break;
        case 3:
            direction += 0.007 * (Vec2f(100.0f, 100.0f) - location);
            life -= 25;
            break;
        default:
            break;
    }
}

void Particle::draw()
{
    //cairo::Context ctx( cairo::createWindowSurface() );
    //	ctx.setLineWidth( 2.5f );
    //ctx.setSourceRgb( 1.0f, 0.5f, 0.25f );
    ////ctx.appendPath( Path2d( BSpline2f( mPoints, mDegree, mLoop, mOpen ) ) );
    //ctx.stroke();
    //  //gl::drawSolidCircle(location, radius);
    
    gl::lineWidth(6.0f);
    gl::color(ColorA(1.0,1.0,1.0,1.0));
    if(pastPoints.size() > 0)
    {
        for(int i = 0; i < pastPoints.size() - 1; i++)
        {
            gl::color(ColorA(color.r, color.g - 0.05f * i, color.b, color.a));
            gl::drawLine(pastPoints[i], pastPoints[i+1]);
            gl::lineWidth(6.0f - 0.6f * i);
        }
    }
}

bool Particle::isDead()
{
    return life <= 0;
}
