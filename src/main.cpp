#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
    ofSetCurrentRenderer(ofGLProgrammableRenderer::TYPE);
	ofSetupOpenGL(512, 512, OF_WINDOW);
    ofRunApp(new ofApp());
}
