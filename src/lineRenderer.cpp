#include "lineRenderer.h"

LineRenderer::LineRenderer()
{
    
}

void LineRenderer::setup()
{
    calculateJointCentroid();
    calculateAreaMinMax();
}

void LineRenderer::update()
{
    for (auto &lf: m_lineFollowers)
    {
        lf.update();
    }
}

void LineRenderer::draw()
{
    for (auto &lf: m_lineFollowers)
    {
        //should be calculating some noise color here
        lf.draw();
    }
}

void LineRenderer::drawDebug()
{
    ofPushStyle();
    ofSetColor(ofColor::red);
    for (auto &lf: m_lineFollowers)
    {
        lf.drawDebug();
    }
    ofCircle(m_jointCentroid, 8);
    ofPopStyle();
}

void LineRenderer::addFollower(LineFollower &follower)
{
    m_lineFollowers.push_back(follower);
}

void LineRenderer::calculateJointCentroid()
{
    //----Here, we calculate the bounding box that encloses ALL of the polylines
    float minX = ofGetWidth();
    float minY = ofGetHeight();
    float maxX = 0;
    float maxY = 0;
    for (auto &lf: m_lineFollowers)
    {
        ofRectangle rect = lf.getPath().getBoundingBox();
        if (rect.x < minX) minX = rect.x;
        if (rect.y < minY) minY = rect.y;
        if (rect.x > maxX) maxX = rect.x;
        if (rect.y > maxY) maxY = rect.y;
    }
   m_jointCentroid.set((minX + maxX) / 2,
                       (minY + maxY) / 2);
}

void LineRenderer::calculateAreaMinMax()
{
    float min = ofGetWidth() * ofGetHeight();
    float max = 0;
    for (auto &lf: m_lineFollowers)
    {
        float lfArea = lf.getPath().getArea();
        if (lfArea < min) min = lfArea;
        if (lfArea > max) max = lfArea;
    }
    m_minArea = min;
    m_maxArea = max;
}

void LineRenderer::clear()
{
    m_lineFollowers.clear();
}