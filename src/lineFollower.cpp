#include "lineFollower.h"

LineFollower::LineFollower()
{
    
}

void LineFollower::setupFromPoints(vector<ofPoint> &points)
{
    ofPolyline line;
    for (auto &point: points)
    {
        line.addVertex(point);
    }
    
    //----Path related vars
    m_path = line;
    m_area = m_path.getArea();
    m_perimeterLength = m_path.getPerimeter();
    
    //----General settings
    m_noiseOffset = ofRandom(1000.0f);
    m_radius = 10.0f;
    m_travelSpeed = 0.03f * (100.0f / m_perimeterLength);
    m_pctTraveled = 0.0f;
    m_currentPosition = line.getPointAtPercent(m_pctTraveled);
    
    //----This is primarily for debugging
    m_shouldDrawPath = false;
}

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
    //r *= 100 / m_area;
    ofCircle(m_currentPosition, r);
    
    //----Secondary stroke
    if (ofRandom(1.0) > 0.95)
    {
        float offsetX = ofRandom(-10.0, 10.0);
        float offsetY = ofRandom(-10.0, 10.0);
        ofCircle(m_currentPosition.x + offsetX, m_currentPosition.y + offsetY, r / 2, r / 2);
    }
}

void LineFollower::drawDebug()
{
    ofPushStyle();
    ofSetColor(ofColor::red);
    m_path.draw();
    ofCircle(m_currentPosition, 4);
    ofPopStyle();
}
