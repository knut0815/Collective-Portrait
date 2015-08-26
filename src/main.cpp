#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
    ofSetCurrentRenderer(ofGLProgrammableRenderer::TYPE);
	ofSetupOpenGL(1080, 1920, OF_WINDOW);
    ofRunApp(new ofApp());
}
