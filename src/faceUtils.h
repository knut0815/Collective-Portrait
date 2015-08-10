#ifndef __collectivePortrait__faceUtils__
#define __collectivePortrait__faceUtils__

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "lineFollower.h"
#include "inkRenderer.h"

class FaceUtils {
public:
    FaceUtils();
    void setup(int width, int height, InkRenderer *inkRenderer);
    void update();
    void draw();
    void drawDebug();
    void reset();
    void setThreshold(int threshold);
    void setMinBlobArea(int blobArea);
    void setMaxBlobArea(int blobArea);
    
private:
    //----General vars
    int m_width;
    int m_height;
    int m_minHaarArea;
    double m_haarTimer;
    double m_drawDuration;
    bool m_isDrawing;
    
    //----Settings
    bool                        m_shouldDetectMotion;
    bool                        m_learnBackground;
    float                       m_threshold;
    int                         m_nonZero;
    int                         m_minBlobArea;
    int                         m_maxBlobArea;
    
    //----Face tracking and OpenCV
    ofVideoGrabber              m_grabber;
    ofxCvHaarFinder             m_haarFinder;
    ofxCvContourFinder          m_contourFinder;
    ofxCvColorImage             m_grabberCvColor;
    ofxCvGrayscaleImage         m_grabberCvGrayscale;
    ofxCvGrayscaleImage         m_backgroundCvGrayscale;
    ofxCvGrayscaleImage         m_diffCvGrayscale;
    
    vector<ofRectangle>         m_haarBoundingRects;
    vector<LineFollower>        m_followers;
    
    //----Other
    InkRenderer                 *m_inkRenderer;
};

#endif
