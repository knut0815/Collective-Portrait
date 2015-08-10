#include "faceUtils.h"

FaceUtils::FaceUtils()
{
    
}

void FaceUtils::setup(int width, int height, InkRenderer *inkRenderer)
{
    m_width =               width;
    m_height =              height;
    m_minHaarArea =         (width * height) / 10;
    m_drawDuration =        20.0;
    
    m_grabber.initGrabber(m_width, m_height);
    m_haarFinder.setup("haarcascade_frontalface_default.xml");
    
    //----Allocate space for CV images
    m_grabberCvColor.allocate(m_width, m_height);
    m_grabberCvGrayscale.allocate(m_width, m_height);
    m_backgroundCvGrayscale.allocate(m_width, m_height);
    m_diffCvGrayscale.allocate(m_width, m_height);
    
    //----Default settings
    m_shouldDetectMotion =  true;
    m_learnBackground =     true;
    m_threshold =           70;
    m_nonZero =             0;
    m_minBlobArea =         10;
    m_maxBlobArea =         (m_width * m_height) / 8;
    m_inkRenderer =         inkRenderer;
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
            m_nonZero = m_diffCvGrayscale.countNonZeroInRegion(0, 0, m_width, m_height);
            
            //----Perform face detection if this number exceeds a certain threshold
            if (m_nonZero > 10000) //This is arbitrary rn
            {
                cout << "Running the Haar finder..." << endl;
                m_haarFinder.findHaarObjects(m_grabberCvGrayscale);
                
                //----If we found any faces...
                if (m_haarFinder.blobs.size() > 0)
                {
                    cout << "Found " + ofToString(m_haarFinder.blobs.size()) << " potential faces." << endl;
    
                    bool foundValidHaar = false;
                    
                    for (auto &blob: m_haarFinder.blobs)
                    {
                        ofRectangle rect = blob.boundingRect;
                        if (rect.getArea() >= m_minHaarArea)
                        {
                            m_haarBoundingRects.push_back(rect);
                            foundValidHaar = true;
                        }
                    }
                    
                    if (foundValidHaar)
                    {
                        //----Since we found at least 1 valid face, we won't attempt presence detection next frame
                        m_shouldDetectMotion = false;
                        m_isDrawing = true;
                        m_haarTimer = ofGetElapsedTimef();
                        
                        //----Threshold the current frame (grayscale) and perform contour detection
                        m_grabberCvGrayscale.contrastStretch();
                        
                        //----We thresold the image until we have reached a 50:50 ratio of black to white pixels
                        ofxCvGrayscaleImage adaptiveTImage;
                        int threshold = 120;
                        int thresholdStep = 5;
                        while (true)
                        {
                            adaptiveTImage = m_grabberCvGrayscale;
                            adaptiveTImage.threshold(threshold);
                            cout << "Thresholding at " << threshold << "\n";
                            
                            int sum = 0;
                            int totalArea = 0;
                            
                            for (auto &r: m_haarBoundingRects)
                            {
                                sum += adaptiveTImage.countNonZeroInRegion(r.x, r.y, r.width, r.height);
                                totalArea += r.getArea();
                            }
                            
                            if (sum >= (totalArea / 2))
                            {
                                break;
                            }
                            else
                            {
                                threshold -= thresholdStep;
                            }
                            
                        }
                        
                        /* All function calls below use to be m_grabberCvGrayscale */
                        adaptiveTImage.invert();
                        m_contourFinder.findContours(adaptiveTImage, m_minBlobArea, m_maxBlobArea, 20, false);
                        
                        
                        //----Find all CV blobs that are within the facial region and convert them to polylines
                        for (auto &rect: m_haarBoundingRects)
                        {
                            for (auto &blob: m_contourFinder.blobs)
                            {
                                if (rect.inside(blob.centroid) || rect.inside(blob.centroid.x, blob.centroid.y - (rect.height/2))) // Fudge factor for neck blobs
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
                }
            }
        }
    }
    
    //----Reset the simulation if we've drawn for the specified duration
    if ((ofGetElapsedTimef() - m_haarTimer >= m_drawDuration) && m_isDrawing) reset();
}

void FaceUtils::drawDebug()
{
    //----Red path outline and point position
    for (auto &f: m_followers) {
        f.drawDebug();
    }
    
    ofPushStyle();
    ofNoFill();
    ofSetColor(ofColor::red);
    for (auto &r: m_haarBoundingRects)
    {
        ofRect(r.x, r.y, r.width, r.height);
    }
    ofPopStyle();
    
    //----Text
    ofPushStyle();
    ofSetColor(60, 60, 60);
    ofDrawBitmapString("FPS: " + ofToString(ofGetFrameRate()) + "\n" +
                       "Threshold: " + ofToString(m_threshold) + "\n" +
                       "Min Blob Area: " + ofToString(m_minBlobArea) + "\n" +
                       "Max Blob Area: " + ofToString(m_maxBlobArea) + "\n" +
                       "Resets every " + ofToString(m_drawDuration) + " seconds. Time elapsed: " + ofToString(ofGetElapsedTimef() - m_haarTimer),
                       10, 14);
    ofPopStyle();
}

void FaceUtils::reset()
{
    // I dont think we want to do this...it should probably happen a few times throughout the day. m_learnBackground = true;
    m_shouldDetectMotion = true;
    m_isDrawing = false;
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