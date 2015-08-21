#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "inkRenderer.h"
#include "lineFollower.h"
#include "faceScanner.h"
#define DEBUG
class ofApp : public ofBaseApp{
    
public:
    
    void setup();
    void exit();
    void update();
    void draw();
    void keyPressed(int key);
    
    InkRenderer                 ink;
    FaceScanner                 scanner;
    int                         m_drawTimeout;
    bool                        m_drawDebug;
};
