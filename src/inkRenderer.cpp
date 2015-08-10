#include "inkRenderer.h"

InkRenderer::InkRenderer()
{
    //----Empty constructor
}

void InkRenderer::prepareFbos()
{
    const ofColor rgGray(128, 128, 0);
    
    //----Allocate and clear all FBOs
    m_inkBlurFbo.allocate(m_width, m_height, GL_RGBA32F, m_precision);
    m_inkBlurFbo.begin();
    ofClear(m_backgroundColor);
    m_inkBlurFbo.end();
    
    m_ping.allocate(m_width, m_height, GL_RGBA32F, m_precision);
    m_ping.begin();
    ofClear(rgGray); //50% gray
    m_ping.end();
    
    m_pong.allocate(m_width, m_height, GL_RGBA32F, m_precision);
    m_pong.begin();
    ofClear(rgGray); //50% gray
    m_pong.end();
    
    m_paintFbo.allocate(m_width, m_height, GL_RGBA32F, m_precision);
    m_paintFbo.begin();
    ofClear(ofColor::white);
    m_paintFbo.end();
    
    m_blurXFbo.allocate(m_width, m_height, GL_RGBA32F, m_precision);
    m_blurXFbo.begin();
    ofClear(ofColor::white);
    m_blurXFbo.end();
    
    m_blurYFbo.allocate(m_width, m_height, GL_RGBA32F, m_precision);
    m_blurYFbo.begin();
    ofClear(ofColor::white);
    m_blurYFbo.end();
    
    m_edgeFbo.allocate(m_width, m_height, GL_RGBA32F, m_precision);
    m_edgeFbo.begin();
    ofClear(ofColor::white);
    m_edgeFbo.end();
}

void InkRenderer::setup(int width = 512, int height = 512, int precision = 4) {
    //----General settings
    m_width =           width;
    m_height =          height;
    m_precision =       precision;
    m_isRendering =     true;
    
    //----Clamp the precision of the FBOs
    if (m_precision > MAX_FBO_PRECISION) {
        m_precision =   MAX_FBO_PRECISION;
    }
    else if (m_precision < MIN_FBO_PRECISION) {
        m_precision =   MIN_FBO_PRECISION;
    }
    
    //----Colors and FBO allocation
    m_inkColor[0] =     0.3;
    m_inkColor[1] =     0.5;
    m_inkColor[2] =     0.8;
    m_backgroundColor.set(249, 250, 234);
    prepareFbos();
    
    //----Load all of the shaders from the bin/data/shaders folder
    loadShaders();
    
    //----Noise params
    m_noiseSpeed =      0.01f;
    m_noiseOffset =     1000.0f;
    
    //----Brush settings
    m_offsetSplatter =  20.0f;
    m_pctSplatter =     0.8f;
    m_minBrushSize =    10.0f;
    m_maxBrushSize =    50.0f;
    
    //----Shader uniforms
    m_blurAmount =          1.5f / m_width; //1.5
    m_displacementAmount =  2.0f; //2.0f
    m_displacementSpeed =   0.01f; //0.01f
    
    //----Textures
    m_backgroundImage.loadImage("paper.jpg");
    m_brushImage.loadImage("brush.png");
    
    if (!m_backgroundImage.isAllocated() || !m_brushImage.isAllocated())
    {
        ofLogError("Ink Renderer") << "Warning: no background and/or brush image has been loaded...defaulting to a white.";
        m_backgroundImage.allocate(m_width, m_height, OF_IMAGE_GRAYSCALE);
        m_backgroundImage.setColor(ofColor::white);
        m_brushImage.allocate(80, 80, OF_IMAGE_GRAYSCALE);
        m_brushImage.setColor(ofColor::white);
    }
    m_drawMode = MANUAL;
}

void InkRenderer::setBackgroundTexture(ofImage &background)
{
    m_backgroundImage = background;
}

void InkRenderer::setBrushTexture(ofImage &brush)
{
    m_brushImage = brush;
}

void InkRenderer::setLineFollowers(vector<LineFollower> &followers)
{
    m_followers = followers;
    calculateOffset();
}

void InkRenderer::calculateOffset()
{
    //----Here, we calculate the bounding box that encloses ALL of the polylines
    float minX = ofGetWidth();
    float minY = ofGetHeight();
    float maxX = 0;
    float maxY = 0;
    for (auto &lf: m_followers) {
        ofRectangle rect = lf.getPath().getBoundingBox();
        if (rect.x < minX) minX = rect.x;
        if (rect.y < minY) minY = rect.y;
        if (rect.x > maxX) maxX = rect.x;
        if (rect.y > maxY) maxY = rect.y;
    }
    m_totalCentroid.set((minX + maxX) / 2,
                        (minY + maxY) / 2);
    
}

void InkRenderer::loadShaders()
{
    m_displacementShader.load("shaders/passthrough.vert", "shaders/displacement.frag");
    m_renderShader.load("shaders/passthrough.vert", "shaders/render.frag");
    m_multiplyShader.load("shaders/passthrough.vert", "shaders/multiply.frag");
    m_paintShader.load("shaders/passthrough.vert", "shaders/paintstroke.frag");
    m_blurXShader.load("shaders/passthrough.vert", "shaders/blurX.frag");
    m_blurYShader.load("shaders/passthrough.vert", "shaders/blurY.frag");
    m_edgeShader.load("shaders/passthrough.vert", "shaders/edgedetect.frag");
}

void InkRenderer::update()
{
    //----Only update the followers if we are in the proper draw mode
    if (m_drawMode == FOLLOWERS)
    {
        for (auto &f: m_followers)
        {
            f.update();
        }
    }
    
    //----If any key is pressed, we erase
    if (ofGetKeyPressed()) setDrawMode(ERASE);
    else setDrawMode(FOLLOWERS);
    
    //----Swap the ping / pong FBOs
    ofFbo temp;
    temp = m_ping;
    m_ping = m_pong;
    m_pong = temp;
    
    //----Draw into the ping FBO
    m_ping.begin();
    ofPushStyle();
    if(m_isRendering)
    {
        if (m_drawMode == MANUAL)
        {
            float nx = ofNoise(ofGetFrameNum() * 0.01) * 255.0;
            float ny = ofNoise(ofGetFrameNum() * 0.01 + 1000.0) * 255.0;
            
            ofSetColor(nx, ny, 255, 50);
        
            float r = ofRandom(m_minBrushSize, m_maxBrushSize);
            m_brushImage.draw(ofGetMouseX() - r/2, ofGetMouseY() - r/2, r, r);
            
            //----Secondary stroke
            if (ofRandom(1.0) > m_pctSplatter)
            {
                float offsetX = ofRandom(-m_offsetSplatter, m_offsetSplatter);
                float offsetY = ofRandom(-m_offsetSplatter, m_offsetSplatter);
                m_brushImage.draw(ofGetMouseX() - r/4 + offsetX, ofGetMouseY() - r/4 + offsetY, r / 2, r / 2);
            }
        }
        else if (m_drawMode == FOLLOWERS && !m_followers.empty())
        {
            for (int i = 0; i < m_followers.size(); i++)
            {
                //----Calculate the current object's fill color from two independent noise values
                float nx = ofNoise(ofGetFrameNum() * 0.01 + i) * 255.0;
                float ny = ofNoise(ofGetFrameNum() * 0.01 + i + 1000.0) * 255.0;
                ofSetColor(nx, ny, 255, 50);
                
                ofPushMatrix();
                ofTranslate(ofGetWidth()/2 - m_totalCentroid.x, ofGetHeight()/2 - m_totalCentroid.y);
                m_followers[i].draw();
                ofPopMatrix();
            }
        }
        else if (m_drawMode == ERASE)
        {
            //----If we are erasing, simply draw 50% gray into the FBO
            ofSetColor(128, 128, 0);
            float r = ofRandom(m_minBrushSize, m_maxBrushSize);
            m_brushImage.draw(ofGetMouseX() - r/2, ofGetMouseY() - r/2, r, r);
        }
    }
    ofPopStyle();
    m_ping.end();
    
    //----Render the ping FBO into the pong FBO after applying the displacement shader
    m_pong.begin();
    ofClear(255, 255, 255, 0);
    m_displacementShader.begin();
    m_displacementShader.setUniformTexture("tex0", m_ping.getTextureReference(), 0);
    m_displacementShader.setUniform1f("displacementAmount", m_displacementAmount);
    m_displacementShader.setUniform1f("displacementSpeed", m_displacementSpeed);
    m_ping.draw(0, 0);
    m_displacementShader.end();
    m_pong.end();
}


void InkRenderer::draw()
{
    //----Apply the render shader
    m_inkBlurFbo.begin();
    m_renderShader.begin();
    m_renderShader.setUniform1f("time", ofGetElapsedTimef());
    m_renderShader.setUniform3fv("inkColor", m_inkColor);
    m_pong.draw(0, 0);
    m_renderShader.end();
    m_inkBlurFbo.end();
    
    //----Paint stroke shader
    m_paintFbo.begin();
    m_paintShader.begin();
    m_paintShader.setUniformTexture("inputTexture", m_inkBlurFbo.getTextureReference(), 0);
    m_inkBlurFbo.draw(0, 0);
    m_paintShader.end();
    m_paintFbo.end();
    
    //----X / Y blurring
    m_blurXFbo.begin();
    m_blurXShader.begin();
    m_blurXShader.setUniformTexture("tex0", m_paintFbo.getTextureReference(), 0);
    m_blurXShader.setUniform1f("blurAmount", m_blurAmount);
    m_paintFbo.draw(0, 0);
    m_blurXShader.end();
    m_blurXFbo.end();
    
    m_blurYFbo.begin();
    m_blurYShader.begin();
    m_blurYShader.setUniformTexture("tex0", m_blurXFbo.getTextureReference(), 0);
    m_blurYShader.setUniform1f("blurAmount", m_blurAmount);
    m_blurXFbo.draw(0, 0);
    m_blurYShader.end();
    m_blurYFbo.end();
    
    //----Edge detection on the original paint stroke FBO
    m_edgeFbo.begin();
    m_edgeShader.begin();
    m_edgeShader.setUniformTexture("inputTexture", m_paintFbo.getTextureReference(), 0);
    m_paintFbo.draw(0,0);
    m_edgeShader.end();
    m_edgeFbo.end();
    
    //----Finally, multiply the result into the paper texture
    m_multiplyShader.begin();
    m_multiplyShader.setUniformTexture("tex0", m_blurYFbo.getTextureReference(), 0);
    m_multiplyShader.setUniformTexture("tex1", m_inkBlurFbo.getTextureReference(), 1);
    m_multiplyShader.setUniformTexture("tex2", m_edgeFbo.getTextureReference(), 2);
    m_multiplyShader.setUniformTexture("tex3", m_backgroundImage.getTextureReference(), 3);
    m_multiplyShader.setUniform1f("time", ofGetElapsedTimef());
    m_blurYFbo.draw(0, 0);
    m_multiplyShader.end();
    
    ofSetColor(ofColor::rosyBrown);
    ofCircle(m_totalCentroid, 10);
}



void InkRenderer::begin()
{
    m_ping.begin();
}

void InkRenderer::end()
{
    m_ping.end();
}

void InkRenderer::clear()
{
    m_ping.begin();
    ofClear(128, 128, 0);
    m_ping.end();
    
    m_pong.begin();
    ofClear(128, 128, 0);
    m_pong.end();
    
    m_followers.clear();
}

void InkRenderer::setDrawMode(DRAWMODE mode)
{
    m_drawMode = mode;
}
