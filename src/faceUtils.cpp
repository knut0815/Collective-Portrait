#include "faceUtils.h"

FaceUtils::FaceUtils()
{
    
}

void FaceUtils::setup(int width, int height, InkRenderer *inkRenderer)
{
    m_width = width;
    m_height = height;
    m_grabber.initGrabber(m_width, m_height);
    m_haarFinder.setup("haarcascade_frontalface_default.xml");
    
    //----Allocate space for CV images
    m_grabberCvColor.allocate(m_width, m_height);
    m_grabberCvGrayscale.allocate(m_width, m_height);
    m_backgroundCvGrayscale.allocate(m_width, m_height);
    m_diffCvGrayscale.allocate(m_width, m_height);
    
    //----Default settings
    m_shouldHaarRun = true;
    m_shouldDetectMotion = true;
    m_learnBackground = true;
    m_threshold = 70;
    m_nonZero = 0;
    m_minBlobArea = 10;
    m_maxBlobArea = (m_width * m_height) / 8;
    
    m_inkRenderer = inkRenderer;
}

void FaceUtils::update()
{
    m_grabber.update();
    
    if (m_grabber.isFrameNew())
    {
        //----Should we even detect presence?
        if (m_shouldDetectMotion)
        {
            //----Put the grabber's pixels into a Color CV image and immediately convert it to grayscale
            m_grabberCvColor.setFromPixels(m_grabber.getPixels(), m_width, m_height);
            m_grabberCvGrayscale = m_grabberCvColor;
            
            if (m_learnBackground || ofGetElapsedTimef() < 1.5)
            {
                m_backgroundCvGrayscale = m_grabberCvGrayscale;
                m_learnBackground = false;
            }
            
            //----This computes the difference between the 'learned' background and the current frame - we also threshold it
            m_diffCvGrayscale.absDiff(m_backgroundCvGrayscale, m_grabberCvGrayscale);
            m_diffCvGrayscale.threshold(m_threshold);
            
            //----Count the number of white pixels
            m_nonZero = m_diffCvGrayscale.countNonZeroInRegion(0, 0, m_width, m_height);
            
            //----Perform face detection if this number exceeds a certain threshold
            if (m_nonZero > 3000) //This is arbitrary rn
            {
                if (m_shouldHaarRun)
                {
                    cout << "Running the Haar finder..." << endl;
                    m_haarFinder.findHaarObjects(m_grabberCvGrayscale);
                    
                    //----If we found any faces...
                    if (m_haarFinder.blobs.size() > 0)
                    {
                        cout << "Found " + ofToString(m_haarFinder.blobs.size()) << " potential faces." << endl;
                        for (int i = 0; i < m_haarFinder.blobs.size(); i++) {
                            ofRectangle rect = m_haarFinder.blobs[i].boundingRect;
                            m_haarBoundingRects.push_back(rect);
                        }
                        
                        //----Threshold the current frame (grayscale) and perform contour detection
                        m_grabberCvGrayscale.contrastStretch();
                        m_grabberCvGrayscale.threshold(m_threshold);
                        //m_grabberCvGrayscale.adaptiveThreshold(30);
                    
                        m_grabberCvGrayscale.invert();
                        m_contourFinder.findContours(m_grabberCvGrayscale, m_minBlobArea, m_maxBlobArea, 10, false);
                    
                        
                        m_shouldDetectMotion = false;
                        
                        //----Find all CV blobs that are within the facial region and convert them to polylines
                        for (auto &rect: m_haarBoundingRects)
                        {
                            //ofRect(rect.x, rect.y, rect.width, rect.height);
                            for (auto &blob: m_contourFinder.blobs)
                            {
                                if (rect.inside(blob.centroid) || rect.inside(blob.centroid.x, blob.centroid.y - 100)) // Fudge factor for neck blobs
                                {
                                    //----Add a new line follower object from this blob's points
                                    LineFollower lf;
                                    lf.setupFromPoints(blob.pts);
                                    m_followers.push_back(lf);
                                }
                            }
                            m_inkRenderer->setLineFollowers(m_followers);
                        }
                    }
                    m_shouldHaarRun = false;
                }
            }
        }
    }
}

void FaceUtils::drawDebug()
{
    //----Red path outline and point position
    for (auto &f: m_followers) {
        f.drawDebug();
    }
    
    //----Text
    ofPushStyle();
    ofSetColor(60, 60, 60);
    ofDrawBitmapString("FPS: " + ofToString(ofGetFrameRate()) + "\n" +
                       "Threshold: " + ofToString(m_threshold) + "\n" +
                       "Min Blob Area: " + ofToString(m_minBlobArea) + "\n" +
                       "Max Blob Area: " + ofToString(m_maxBlobArea),
                       10, 14);
    ofPopStyle();
}

void FaceUtils::reset()
{
    m_learnBackground = true;
    m_shouldHaarRun = true;
    m_shouldDetectMotion = true;
    m_haarBoundingRects.clear();
    m_followers.clear();
    
    //why do i have to clear both vectors?
    m_inkRenderer->clear();
}

void FaceUtils::setThreshold(int threshold)
{
    m_threshold = threshold;
}

void FaceUtils::setMinBlobArea(int blobArea)
{
    m_minBlobArea = blobArea;
}

void FaceUtils::setMaxBlobArea(int blobArea)
{
    m_maxBlobArea = blobArea;
}