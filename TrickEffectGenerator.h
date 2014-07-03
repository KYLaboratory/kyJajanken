//
//  TrickEffectGenerator.h
//  JajankenHunter
//
//  Created by akihito on 2014/06/20.
//
//

#ifndef __JajankenHunter__TrickEffectGenerator__
#define __JajankenHunter__TrickEffectGenerator__

#include "cinder/Vector.h"
#include "cinder/gl/Texture.h"

class TrickEffectGenerator;
typedef std::shared_ptr<TrickEffectGenerator> TrickEffectGeneratorRef;

enum TRICK{
    NONE,
    ROCK,
    SCISSORS,
    PAPER,
};

class TrickEffectGenerator
{
    const int width;
    const int height;
    
    const int memberID;
    
    ci::gl::TextureRef mTexture;
    
    TRICK trick;
    ci::Vec2i textPosition;
    
public:
    TrickEffectGenerator(const int width, const int height);
    ~TrickEffectGenerator();

    static TrickEffectGeneratorRef create(const int width, const int height);
    
    void start(TRICK trick);
    void update();
    ci::gl::TextureRef getTexture();
};

#endif /* defined(__JajankenHunter__TrickEffectGenerator__) */
