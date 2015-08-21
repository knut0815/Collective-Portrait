#include "faceScanner.h"

FaceScanner::FaceScanner()
{
    
}

void FaceScanner::setup(int width, int height, InkRenderer *ink) {
    m_width =               width;
    m_height =              height;
    m_shouldTrack =         true;
    m_shouldClearAmbient =  true;
    m_drawTimeout =         30.0f;       //The number of seconds before the FaceScanner will try to find a new face to scan
    m_ambientTimeout =      5.0f;        //The number of (idle) seconds before the InkRenderer will switch to a particle system
    m_ambientFrameStart =   0.0f;
    
    for (auto &device: m_grabber.listDevices())
    {
        ofLogNotice("Face Scanner") << "Device found: " << device.deviceName << ", ID: " << device.id;
    }
    
    m_grabber.setDeviceID(0);
    m_grabber.initGrabber(m_width, m_height);
    if (!m_grabber.isInitialized())
    {
        ofLogError("Face Scanner") << "Failed to initialize the video grabber.";
    }
    m_tracker.setup();
    m_thresh.allocate(m_width, m_height, OF_IMAGE_GRAYSCALE);
    m_threshCopy.allocate(m_width, m_height, OF_IMAGE_GRAYSCALE);
    m_contourFinder.setMinAreaRadius(MIN_CONTOUR_AREA);
    m_contourFinder.setMaxAreaRadius(MAX_CONTOUR_AREA);
    
    //----We need a pointer to the ink renderer so that we can pass the contours over for rendering
    m_inkRenderer =         ink;
}

void FaceScanner::update()
{
    m_grabber.update();
    
    if(m_grabber.isFrameNew())
    {
        if (m_shouldTrack)
        {
            m_tracker.update(toCv(m_grabber));
            
            //----If we've found a face, we store its intrinsics and begin our scanning procedure
            if (m_tracker.getFound() && ofGetElapsedTimef() > 2.0f)
            {
                ofLogNotice("Face Scanner") << "Found a face.";
                
                //----The FBOs are cleared IFF the last sequence drawn was the particle system (denoted by the boolean m_shouldClearAmbient)
                if (!m_shouldClearAmbient) m_inkRenderer->clear();
                
                m_inkRenderer->setDrawMode(InkRenderer::FOLLOWERS);
                convertColor(m_grabber, m_thresh, CV_RGB2GRAY);
                m_faceOutline =         m_tracker.getImageFeature(ofxFaceTracker::FACE_OUTLINE);
                m_faceCenter =          m_faceOutline.getCentroid2D();
                m_faceArea =            m_faceOutline.getArea() * -1;       //This is a hack: something is wrong with the sign of the value returned by getArea()
                m_shouldTrack =         false;
                m_drawFrameStart =      ofGetElapsedTimef();                //When did this scan start?
                
                scan(200, 5);                                              //Start at a threshold value of 200, and decrement by 20 for each iteration
            }
            else
            {
                //----If we don't see a face and it's been m_ambientTimeout seconds, enter ambient mode
                if ((ofGetElapsedTimef() - m_ambientFrameStart) >= m_ambientTimeout) {
                    
                    //----We only want to do these operations once, otherwise the FBOs will clear every frame, and the particle system will never be drawn
                    if (m_shouldClearAmbient)
                    {
                        ofLogNotice("Face Scanner") << "Entering ambient mode.";
                        
                        //----We tell the InkRenderer to draw particles after clearing the FBOs and resetting the "draw counter"
                        //----By resetting the "draw counter", we ensure that the next time the FaceScanner starts detecting faces, it'll draw 3 in a row
                        m_inkRenderer->setDrawMode(InkRenderer::PARTICLES);
                        m_inkRenderer->resetNumDrawingsCompleted();
                        m_inkRenderer->clear();
                        m_shouldClearAmbient = false;
                    }
                }
            }
        }
        else if ((ofGetElapsedTimef() - m_drawFrameStart) >= m_drawTimeout)
        {
            //----If we shouldn't be tracking, that means we've already found a face, so begin the countdown
            ofLogNotice("Face Scanner") << "Starting a new scan.";
            
            //----After this point, we might not see another face, so we record the current time and ready the InkRenderer for a particle simulation
            m_ambientFrameStart =   ofGetElapsedTimef();
            m_shouldClearAmbient =  true;
            ofSaveScreen("screenshots/image_" + ofGetTimestampString() + ".png");
            
            reset();
        }
    }
}

void FaceScanner::draw()
{
    //----These are primarily for debugging purposes
    ofPushStyle();
    
    ofSetColor(ofColor::white);
    m_grabber.draw(10, 10, m_width/4, m_height/4);
    
    ofSetColor(ofColor::blue);
    ofCircle(m_faceCenter, 4);
    m_faceOutline.draw();
    
    ofPopStyle();
}

void FaceScanner::exit()
{
    ofLogNotice("Face Scanner") << "Exiting the face tracking thread.";
    m_tracker.waitForThread();
}

void FaceScanner::scan(int levels, int step)
{
    //----We adaptively threshold the captured image for high resolution facial details
    for (int t = 0; t < levels; t += step)
    {
        m_threshCopy = m_thresh;
        threshold(m_threshCopy, t);
        invert(m_threshCopy);
        m_threshCopy.update();
        
        m_contourFinder.findContours(m_threshCopy);
        
        //----We check each contour and ensure that it is *close enough* to the face
        for (auto line : m_contourFinder.getPolylines())
        {
            ofPoint lCenter = line.getCentroid2D();
            float lArea = line.getArea() * -1;
            float dist = ofDist(lCenter.x, lCenter.y, m_faceCenter.x, m_faceCenter.y);
            
            if (dist < MIN_DISTANCE_TO_FACE)
            {
                LineFollower lf;
                lf.setupFromPolyline(line);
                m_lineRenderer.addFollower(lf);
            }
        }
    }
    //----Remember, calling the setLineRenderer function increments a counter in the InkRenderer class that counts how many faces we've drawn thus far
    m_lineRenderer.setup();
    m_inkRenderer->setLineRenderer(m_lineRenderer);
}

void FaceScanner::reset()
{
    m_lineRenderer.clear();
    m_shouldTrack =         true;
}