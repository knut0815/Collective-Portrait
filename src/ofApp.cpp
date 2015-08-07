#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    //----Global settings
    ofDisableArbTex();
    ofEnableAlphaBlending();
    ofSetVerticalSync(true);
    
    //----Init ink renderer and utils
    ink.setup(ofGetWidth(), ofGetHeight(), 4);
    utils.setup(640, 480, &ink);
    m_drawDebug = false;
}

//--------------------------------------------------------------
void ofApp::update(){
    ink.update();
    utils.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
    ink.draw();
    if (m_drawDebug) utils.drawDebug();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
//    if(key == OF_KEY_UP) {
//        threshold++;
//        if (threshold > 255.0) threshold = 255.0;
//    }
//    else if (key == OF_KEY_DOWN) {
//        threshold--;
//        if (threshold < 0.0) threshold = 0.0;
//    }
//    else if (key == OF_KEY_LEFT) {
//        m_minBlobArea -= 10;
//        if (m_minBlobArea < 0) m_minBlobArea = 0;
//    }
//    else if (key == OF_KEY_RIGHT) {
//        m_minBlobArea += 10;
//        if (m_minBlobArea > m_maxBlobArea - 1) m_minBlobArea = m_maxBlobArea - 1;
//    }
    if (key == ' ') {
        utils.reset();
    }
    else if (key == 'd' || key == 'D') {
        m_drawDebug = !m_drawDebug;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

