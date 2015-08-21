#include "lineRenderer.h"

LineRenderer::LineRenderer()
{
    
}

void LineRenderer::setup()
{
    calculateJointCentroid();
    calculateAreaMinMax();
    m_paintAlpha = 80.0f;
}

void LineRenderer::update()
{
    for (auto &lf: m_lineFollowers)
    {
        lf.update();
    }
}

bool LineRenderer::isEmpty()
{
    return m_lineFollowers.empty();
}

void LineRenderer::draw()
{
    ofPushMatrix();
    ofTranslate(ofGetWidth()/2 - m_jointCentroid.x, ofGetHeight()/2 - m_jointCentroid.y);
    for (int i = 0; i < m_lineFollowers.size(); i++)
    {
        float nx = ofNoise(ofGetFrameNum() * 0.01f + i) * 255.0f;
        float ny = ofNoise(ofGetFrameNum() * 0.01f + i + 1000.0) * 255.0f;
        ofSetColor(nx, ny, 255, m_paintAlpha);
        m_lineFollowers[i].draw(m_minArea, m_maxArea);
    }
    ofPopMatrix();
}

void LineRenderer::drawDebug()
{
    
    ofSetColor(ofColor::green);
    ofCircle(m_jointCentroid, 8);
    
    ofPushMatrix();
    //----If we add this translation, our debug view will be aligned with the drawing but not the screen-space position of the face outline
    //ofTranslate(ofGetWidth()/2 - m_jointCentroid.x, ofGetHeight()/2 - m_jointCentroid.y);
    
    ofPushStyle();
    ofSetColor(ofColor::red);
    for (auto &lf: m_lineFollowers)
    {
        lf.drawDebug();
    }
    ofPopStyle();
    
    ofPopMatrix();
}

void LineRenderer::addFollower(LineFollower &follower)
{
    m_lineFollowers.push_back(follower);
}

/*
 * This function calculates the joint centroid (i.e. the average of all of the centroids that make up the current render)
 */
void LineRenderer::calculateJointCentroid()
{
    ofPoint sum;
    for (auto &lf: m_lineFollowers)
    {
        sum += lf.getPath().getCentroid2D();
    }
    m_jointCentroid.set(sum / m_lineFollowers.size());
}

/*
 * This function calculates the min / max areas of all of the polylines that make up the current render
 */
void LineRenderer::calculateAreaMinMax()
{
    float min = ofGetWidth() * ofGetHeight();
    float max = 0;
    for (auto &lf: m_lineFollowers)
    {
        float lfArea = lf.getArea();
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

ofPoint LineRenderer::getJointCentroid()
{
    return m_jointCentroid;
}