#ifndef __cardsProject__particle__
#define __cardsProject__particle__

#include "ofMain.h"

class Particle {
public:
    Particle();
    void update();
    void checkBounds();
    void draw();
    void reset();
    
    ofVec2f position;
    ofVec2f velocity;
};

#endif