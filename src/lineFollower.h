#ifndef __collectivePortrait__lineFollower__
#define __collectivePortrait__lineFollower__

#include "ofMain.h"

class LineFollower {
public:
    
    LineFollower();
    void setupFromPoints(vector<ofPoint> &points);
    void update();
    void draw();
    void drawDebug();
    ofPolyline getPath();
    
private:
    
    static const int        MIN_RADIUS = 3;
    static const int        MAX_RADIUS = 8;
    
    ofPolyline              m_path;
    ofPoint                 m_currentPosition;
    float                   m_noiseOffset;
    float                   m_area;
    float                   m_perimeterLength;
    float                   m_radius;
    float                   m_travelSpeed;
    float                   m_pctTraveled;
    bool                    m_shouldDrawPath;
};

#endif
