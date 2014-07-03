#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "Particle.h"
#include "cinder/Rand.h"
#include <list>
#include <math.h>
#include "cinder/ImageIo.h"
#include "cinder/gl/Texture.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class PlayerInfo{
public:
	list<Particle> particles;
    bool isEmitting;
    Vec2i startPos;
	int count;
	int GCP;
	Vec2f beamPos;
	cinder::Color color;

	PlayerInfo(){
		count=0;
		GCP=1;
		isEmitting = false;
	}
};

class testApp : public AppNative {
protected:
	PlayerInfo p1;
	PlayerInfo p2;
	int mode;
 	gl::Texture	mTexture;	
public:
    void setup();
    void mouseDown(MouseEvent event);
    void mouseUp(MouseEvent event);
    void mouseDrag(MouseEvent event);
    void update();
    void draw();
};

void testApp::setup()
{
	mode = 0;
	p1.beamPos= Vec2f(100,100);
	p2.beamPos= Vec2f(400,100);
	p1.color = Color(1.0f, 1.0f, 0.0f);
	p2.color = Color(0.0f, 1.0f, 1.0f);
	mTexture = gl::Texture( loadImage("tame2.jpg"));
}

void testApp::mouseDown( MouseEvent event )
{


	mode =1;
	if(event.isLeft()){
		p1.GCP=1;
	}
	else if(event.isShiftDown()){
		p1.GCP = 3;
	}
	else{
		p1.GCP=2;
	}



	p1.startPos = event.getPos();
	p1.isEmitting = true;


	
	p2.startPos = Vec2f(100, 300);
	p2.isEmitting = true;

}
void testApp::mouseUp(MouseEvent event)
{
	p1.isEmitting = false;
	p2.isEmitting = false;
}
 
void testApp::mouseDrag(MouseEvent event)
{
	
	p1.startPos = event.getPos();
	
}
 
void testApp::update()
{
	p1.count++;
	for (list<Particle>::iterator iter = p1.particles.begin();
         iter != p1.particles.end();) {
        iter->update();
        if (iter->isDead())
            iter = p1.particles.erase(iter);
        else
            iter++;
    }
 
	if (p1.isEmitting) {
        for (int i = 0; i < 120*(1 + 0.5*sin(p1.count*2*3.14/(30))); i++){
			p1.particles.push_back(Particle(p1.startPos + 50*Rand::randVec2f(), p1.GCP,p1.color));
		}
    }




	p2.count++;
	for (list<Particle>::iterator iter = p2.particles.begin();
         iter != p2.particles.end();) {
        iter->update();
        if (iter->isDead())
            iter = p2.particles.erase(iter);
        else
            iter++;
    }
 
	if (p2.isEmitting) {
        for (int i = 0; i < 100*(1 + 0.5*sin(p1.count*2*3.14/(30))); i++){
			p2.particles.push_back(Particle(p2.startPos + 50*Rand::randVec2f(), p2.GCP,p2.color));
		}
    }

}

void testApp::draw()
{
	 gl::clear( Color( 0.0f, 0.0f, 0.0f ) );
	// clear out the window with black
	    // clear out the window with black
	if(mode ==0){
		gl::draw(mTexture, Vec2f( 50, 50 ));
		
	}
 

	for (list<Particle>::iterator iter = p1.particles.begin();
         iter != p1.particles.end();
         iter++)
        iter->draw();

	for (list<Particle>::iterator iter = p2.particles.begin();
         iter != p2.particles.end();
         iter++)
        iter->draw();
}

CINDER_APP_NATIVE( testApp, RendererGl )
