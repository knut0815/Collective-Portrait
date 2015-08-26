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
    void manualScan();
    bool compareBoundingBox(ofPolyline &other, float distThreshold);
    void reset();
    
private:
    
    const int                   MIN_CONTOUR_AREA =              200;    //200
    const int                   MAX_CONTOUR_AREA =              76800;  //This is (640 x 480) / 4
    const int                   MIN_HORZ_DISTANCE_TO_FACE =     50;    //200
    const int                   MIN_VERT_DISTANCE_TO_FACE =     200;
    const int                   MIN_FACE_AREA =                 1500;
    
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
    ofRectangle                 m_faceBoundingBox;
    
    bool                        m_shouldTrack;
    float                       m_drawFrameStart;
    float                       m_drawTimeout;
    float                       m_ambientFrameStart;
    float                       m_ambientTimeout;
    bool                        m_shouldClearAmbient;
    
    LineRenderer                m_lineRenderer;
    InkRenderer                 *m_inkRenderer;
    
};

#endif
