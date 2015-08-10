#ifndef __collectivePortrait__lineRenderer__
#define __collectivePortrait__lineRenderer__

#include "ofMain.h"
#include "lineFollower.h"

class LineRenderer {
public:
    LineRenderer();
    void setup();
    void update();
    void draw();
    void drawDebug();
    void addFollower(LineFollower &follower);
    void calculateJointCentroid();
    void calculateAreaMinMax();
    void clear();
    
private:
    vector<LineFollower>    m_lineFollowers;
    ofPoint                 m_jointCentroid;
    float                   m_minArea;
    float                   m_maxArea;

};

#endif
