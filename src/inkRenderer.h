#ifndef __collectivePortrait__inkRenderer__
#define __collectivePortrait__inkRenderer__

#include "ofMain.h"
#include "lineFollower.h"

class InkRenderer {
public:
    enum                        DRAWMODE {MANUAL, FOLLOWERS, ERASE};
    DRAWMODE                    m_drawMode;
    
    //----Functions and constructors
    InkRenderer();
    void setup(int width, int height, int precision);
    void setBackgroundTexture(ofImage &background);
    void setBrushTexture(ofImage &brush);
    void setLineFollowers(vector<LineFollower> &followers);
    void loadShaders();
    void prepareFbos();
    void update();
    void draw();
    void setDrawMode(DRAWMODE mode);
    
    // Unused
    void begin();
    void end();
    void clear();
private:
    //----General vars and constants
    const int                   MAX_FBO_PRECISION = 16;
    const int                   MIN_FBO_PRECISION = 4;
    int                         m_width;
    int                         m_height;
    int                         m_precision;
    bool                        m_isRendering;
    
    //----Settings
    float                       m_offsetSplatter;
    float                       m_pctSplatter;
    float                       m_noiseSpeed;
    float                       m_noiseOffset;
    float                       m_minBrushSize;
    float                       m_maxBrushSize;
    
    //----FBOs and textures
    float                       m_inkColor[3];
    ofColor                     m_backgroundColor;
    ofImage                     m_backgroundImage;
    ofImage                     m_brushImage;
    ofFbo                       m_inkBlurFbo;
    ofFbo                       m_ping;
    ofFbo                       m_pong;
    ofFbo                       m_renderFbo;
    ofFbo                       m_paintFbo;
    ofFbo                       m_blurXFbo;
    ofFbo                       m_blurYFbo;
    ofFbo                       m_edgeFbo;
    
    //----Shaders
    ofShader                    m_displacementShader;
    ofShader                    m_renderShader;
    ofShader                    m_multiplyShader;
    ofShader                    m_edgeShader;
    ofShader                    m_paintShader;
    ofShader                    m_blurXShader;
    ofShader                    m_blurYShader;
    
    //----Uniforms
    float                       m_blurAmount;
    float                       m_displacementAmount;
    float                       m_displacementSpeed;
    
    //----Geometry
    vector<LineFollower>        m_followers;
};

#endif