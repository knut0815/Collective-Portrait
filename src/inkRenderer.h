#ifndef __collectivePortrait__inkRenderer__
#define __collectivePortrait__inkRenderer__

#include "ofMain.h"
#include "lineRenderer.h"
#include "particleSystem.h"

class InkRenderer {
    
public:
    
    enum                        DRAWMODE {MANUAL, FOLLOWERS, PARTICLES, ERASE};
    DRAWMODE                    m_drawMode;
    
    InkRenderer();
    void setup(int width, int height, int precision);
    void setBackgroundTexture(ofImage &background);
    void setBrushTexture(ofImage &brush);
    void setLineRenderer(LineRenderer &renderer);
    void loadShaders();
    void prepareFbos();
    void update();
    void draw();
    void drawDebug();
    void setDrawMode(DRAWMODE mode);
    void clear();
    
private:
    
    //----General vars and constants
    static const int            MAX_FBO_PRECISION = 16;
    static const int            MIN_FBO_PRECISION = 4;
    static const int            SPLATTER_OFFSET = 20;

    int                         m_width;
    int                         m_height;
    int                         m_precision;
    
    //----Settings
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
    int                         m_paintRadius;
    
    //----Geometry
    LineRenderer                m_lineRenderer;
    ofPoint                     m_drawOffset;
    ParticleSystem              m_ps;
    ofImage                     m_descriptionImage;
    
};

#endif