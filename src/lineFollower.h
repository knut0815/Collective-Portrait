#ifndef __collectivePortrait__lineFollower__
#define __collectivePortrait__lineFollower__

#include "ofMain.h"

class LineFollower {
    
public:
    
    LineFollower();
    void setupFromPoints(vector<ofPoint> &points);
    void setupFromPolyline(ofPolyline &line);
    void init();
    void update();
    void draw();
    void draw(int minArea, int maxArea);
    void drawDebug();
    ofPolyline getPath();
    float getArea();
    
private:
    
    static const int        MIN_RADIUS = 1;
    static const int        MAX_RADIUS = 4;
    static const int        SPLATTER_OFFSET = 10;
    
    ofPolyline              m_path;
    ofPoint                 m_currentPosition;
    float                   m_noiseOffset;
    float                   m_area;
    float                   m_perimeterLength;
    float                   m_travelSpeed;
    float                   m_pctTraveled;
    bool                    m_shouldDrawPath;
    
};

#endif
