#include "lineFollower.h"

LineFollower::LineFollower()
{
    
}

/*
 * Originally, this function was useful for transforming OpenCV blobs (which contained a vector of points)
 * into a LineFollower object. Not so useful anymore but retained for completeness.
 */
void LineFollower::setupFromPoints(vector<ofPoint> &points)
{
    ofPolyline line;
    for (auto &point: points)
    {
        line.addVertex(point);
    }
    m_path = line.getSmoothed(3);
    init();
}

/*
 * This function takes a reference to a pre-existing polyline and creates a LineFollower object. In the context of this 
 * project, the polylines come from the FaceScanner class, which generates a bunch of polylines when it does contour detection.
 */
void LineFollower::setupFromPolyline(ofPolyline &line)
{
    m_path = line.getSmoothed(3);
    init();
}

void LineFollower::init()
{
    m_area =                m_path.getArea() * -1;
    m_perimeterLength =     m_path.getPerimeter();
    m_noiseOffset =         ofRandom(1000.0f);
    m_travelSpeed =         0.03f * (100.0f / m_perimeterLength);
    m_pctTraveled =         0.0f;
    m_currentPosition =     m_path.getPointAtPercent(m_pctTraveled);
    m_shouldDrawPath =      false;
}

/*
 * This function moves the draw location along the polyline, letting us trace its form over time
 */
void LineFollower::update()
{
    m_pctTraveled += m_travelSpeed;
    if (m_pctTraveled > 1.0f) m_pctTraveled = 0.0f;
    m_currentPosition = m_path.getPointAtPercent(m_pctTraveled);
}

void LineFollower::draw()
{
    if (m_shouldDrawPath) m_path.draw();
    float n = ofNoise(ofGetElapsedTimef() + m_noiseOffset);
    float r = ofMap(n, 0, 1, MIN_RADIUS, MAX_RADIUS, true);
    ofCircle(m_currentPosition, r);
    
    //----Secondary stroke
    if (ofRandom(1.0) > 0.95)
    {
        float offsetX = ofRandom(-SPLATTER_OFFSET, SPLATTER_OFFSET);
        float offsetY = ofRandom(-SPLATTER_OFFSET, SPLATTER_OFFSET);
        ofCircle(m_currentPosition.x + offsetX, m_currentPosition.y + offsetY, r / 2, r / 2);
    }
}

/*
 * This version of the draw function takes a MIN and MAX area, which are used to scale the stroke width based on the area of this polyline
 * MIN and MAX are provided by the LineRenderer, which maintains a list of all LineFollower objects
 */
void LineFollower::draw(int minArea, int maxArea)
{
    if (m_shouldDrawPath) m_path.draw();
    float n = ofNoise(ofGetElapsedTimef() + m_noiseOffset);
    float r = ofMap(n, 0, 1, MIN_RADIUS, MAX_RADIUS, true);
    float scaleR = ofMap(m_area, minArea, maxArea, 0.5, 1.0, true);
    r *= scaleR;
    ofCircle(m_currentPosition, r);
    
    //----Secondary stroke
    if (ofRandom(1.0) > 0.95)
    {
        float offsetX = ofRandom(-SPLATTER_OFFSET, SPLATTER_OFFSET);
        float offsetY = ofRandom(-SPLATTER_OFFSET, SPLATTER_OFFSET);
        if (ofRandom(1.0) > 0.98) r *= 4;
        if (ofRandom(1.0) > 0.5) ofSetColor(128, 128, 0);
        ofCircle(m_currentPosition.x + offsetX, m_currentPosition.y + offsetY, r / 2, r / 2);
    }
}

/*
 * This function is useful for debugging and draws the LineFollower's entire path, bounding box, and centroid
 */
void LineFollower::drawDebug()
{
    ofPushStyle();
    ofNoFill();
    m_path.draw();
    ofRectangle rect = m_path.getBoundingBox();
    ofRect(rect.x, rect.y, rect.width, rect.height);
    ofPopStyle();
    
    ofCircle(m_path.getCentroid2D(), 4);
}

float LineFollower::getArea()
{
    return m_area;
}

ofPolyline LineFollower::getPath()
{
    return m_path;
}

