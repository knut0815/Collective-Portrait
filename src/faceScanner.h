#ifndef __faceTrackerTest__faceScanner__
#define __faceTrackerTest__faceScanner__

#include "ofMain.h"
#include "inkRenderer.h"
#include "lineRenderer.h"

#include "ofxCv.h"
using namespace ofxCv;
using namespace cv;

#include "ofxFaceTrackerThreaded.h"

class FaceScanner {
    
public:
    
    FaceScanner();
    void setup(int width, int height, InkRenderer *ink);
    void update();
    void draw();
    void exit();
    void scan(int levels, int step);
    void reset();
    
private:
    
    const int                   MIN_CONTOUR_AREA =      10;
    const int                   MAX_CONTOUR_AREA =      600;
    const int                   MIN_DISTANCE_TO_FACE =  200;
    
    int                         m_width;
    int                         m_height;
    
    ofVideoGrabber              m_grabber;
    ofxFaceTrackerThreaded      m_tracker;
    ContourFinder               m_contourFinder;
    ofImage                     m_thresh;
    ofImage                     m_threshCopy;
    ofPolyline                  m_faceOutline;
    ofPoint                     m_faceCenter;
    float                       m_faceArea;
    
    bool                        m_shouldTrack;
    float                         m_drawFrameStart;
    float                         m_drawTimeout;
    float                         m_ambientFrameStart;
    float                         m_ambientTimeout;
    bool                        m_shouldClearAmbient;
    
    LineRenderer                m_lineRenderer;
    InkRenderer                 *m_inkRenderer;
    
};

#endif
