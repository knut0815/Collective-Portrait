#include "particleSystem.h"

ParticleSystem::ParticleSystem() {
    for (int i = 0; i < NUM_PARTICLES; i++) {
        m_particles.push_back(Particle());
    }
    m_brushImage.loadImage("brush_d.png");
}

void ParticleSystem::update() {
    for(auto &p: m_particles) {
        p.update();
    }
}

void ParticleSystem::draw() {
    for (int i = 0; i < m_particles.size(); i++) {
        for (int j = 0; j < m_particles.size(); j++) {
            ofVec2f sep = m_particles[i].position - m_particles[j].position;
            float dist = sep.length();
            if (dist > 0 && dist < DIST_THRESHOLD) {
                float nx = ofNoise(ofGetFrameNum() * 0.01 + i + j) * 255.0;
                float ny = ofNoise(ofGetFrameNum() * 0.01 + i + j + 1000.0) * 255.0;
                ofSetColor(nx, ny, 255, 4);
                ofLine(m_particles[i].position.x, m_particles[i].position.y, m_particles[j].position.x, m_particles[j].position.y);
                
                float brushSize = ofRandom(1.0, BRUSH_SIZE);
            
                m_brushImage.draw(m_particles[i].position.x - brushSize/2, m_particles[i].position.y - brushSize/2, brushSize, brushSize);
                m_brushImage.draw(m_particles[j].position.x - brushSize/2, m_particles[j].position.y - brushSize/2, brushSize, brushSize);
                
                if (ofRandom(1.0) > 0.95) {
                    float offsetX = ofRandom(-20.0, 20.0);
                    float offsetY = ofRandom(-20.0, 20.0);
                    if (ofRandom(1.0) > 0.5) ofSetColor(128, 128, 0);
                    m_brushImage.draw(m_particles[i].position.x + offsetX - brushSize/4, m_particles[i].position.y + offsetY - brushSize/4, brushSize/4, brushSize/4);
                }
                
            }
        }
    }
}

void ParticleSystem::reset()
{
    for(auto &p: m_particles) {
        p.reset();
    }
}