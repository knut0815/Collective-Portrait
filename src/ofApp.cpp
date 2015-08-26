#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
    ofDisableArbTex();
    ofEnableAlphaBlending();
    ofSetVerticalSync(true);
    
    ink.setup(ofGetWidth(), ofGetHeight(), 1); //WAS 8
    scanner.setup(640, 480, &ink); //was 2* width and height...let's try at this resolution and scale the polylines instead
    m_drawTimeout =         800;
    m_drawDebug =           false;
}

void ofApp::exit()
{
    scanner.exit();
}

//--------------------------------------------------------------
void ofApp::update()
{
    ink.update();
    scanner.update();
}

//--------------------------------------------------------------
void ofApp::draw()
{
    ink.draw();
    if (m_drawDebug)
    {
        ink.drawDebug();
        scanner.draw();
        ofDrawBitmapString("FPS: " + ofToString(ofGetFrameRate()), ofPoint(ofGetWidth()-200, 10));
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
    if (key == ' ') scanner.reset();
    else if (key == 'd' || key == 'D') m_drawDebug = !m_drawDebug;
}
