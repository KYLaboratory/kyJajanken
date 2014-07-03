//
//  Aura.h
//  JajankenHunter
//
//  Created by akihito on 2014/06/20.
//
//

#ifndef __JajankenHunter__AuraEffectGenerator__
#define __JajankenHunter__AuraEffectGenerator__

#include "cinder/Vector.h"
#include "cinder/gl/Texture.h"

class AuraEffectGenerator;
typedef std::shared_ptr<AuraEffectGenerator> AuraEffectGeneratorRef;

class AuraEffectGenerator {
    ci::gl::TextureRef mTexture;
    ci::Vec2i centerCordinate;
    const int width;
    const int height;
public:
    AuraEffectGenerator(const int width, const int height);
    ~AuraEffectGenerator();
    
    static AuraEffectGeneratorRef create(const int width, const int height);
    
    void setCenterCordinate(const ci::Vec2i& cordinate);
    void update();
    ci::gl::TextureRef getTexture();
};


#endif /* defined(__JajankenHunter__AuraEffectGenerator__) */
