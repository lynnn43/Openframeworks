#include "ofApp.h"
#define NUM_PARTICLES 50


ofPoint velocity[NUM_PARTICLES];
ofPoint position[NUM_PARTICLES];
ofColor color[NUM_PARTICLES];
float radius [NUM_PARTICLES];
ofImage pictures[9] ;
ofImage pic1;

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetWindowShape(1024, 768);
    ofSetWindowTitle("particle-starter");
    ofSetFrameRate(60);
    ofBackground(255, 255, 255);
    ofEnableSmoothing();
    ofSetCircleResolution(40);
    
    pic1.load("images/newpic11.jpg");

    for(int i =0; i<9;i++){
        int t = i + 1 ;
        pictures[i].load("images/newpic0" + ofToString(t) + ".jpg");
    }
    
    for(int i =0; i<NUM_PARTICLES; i++){
        radius[i] = ofRandom(10,30);
        
        velocity[i].x = ofRandom(-6, 6);
        velocity[i].y = ofRandom(-6, 6);
        position[i].x = ofGetWidth() / 2.0;
        position[i].y = ofGetHeight() / 2.0;
        
        color[i].r= ofRandom(0,255);
        color[i].g= ofRandom(0,255);
        color[i].b= ofRandom(0,255);
        
    }
    
#ifdef _USE_LIVE_VIDEO
    vidGrabber.setVerbose(true);
    vidGrabber.setup(320,240);
#else
    vidPlayer.load("fingers.mov");
    vidPlayer.play();
    vidPlayer.setLoopState(OF_LOOP_NORMAL);
#endif
    
    colorImg.allocate(320,240);
    grayImage.allocate(320,240);
    grayBg.allocate(320,240);
    grayDiff.allocate(320,240);
    
    bLearnBakground = true;
    threshold = 80;
    
//    Pic1.load("images/Pic1.png");
    
//    for(int i=0; i<NBALLOONS; i++){
//
//        int size = (i+1) * 10; // defining the size of each ball based on its place in the array
//        int randomX = ofRandom( 0, ofGetWidth() ); //generate a random value bigger than 0 and smaller than our application screen width
//        int randomY = ofRandom( 0, ofGetHeight() ); //generate a random value bigger than 0 and smaller than our application screen height
//
//        myBalloon[i].setup(randomX, randomY, size);
//    }
}

//--------------------------------------------------------------
void ofApp::update(){
    ofBackground(100,100,100);
    
    bool bNewFrame = false;
    
    for(int i =0; i<NUM_PARTICLES; i++){
        position[i] += velocity[i];
        
        if(position[i].x < radius[i]) {
            position[i].x = radius[i];
            velocity[i].x *= -1;
        }
        if(position[i].x > ofGetWidth()-radius[i]) {
            position[i].x =ofGetWidth()-radius[i];
            velocity[i].x *= -1;
        }
        if(position[i].y < radius[i]) {
            position[i].y = radius[i];
            velocity[i].y *= -1;
            
        }
        if(position[i].y > ofGetHeight()-radius[i]) {
            position[i].y = ofGetHeight()-radius[i];
            velocity[i].y *= -1;
        }
    }

//  for(int i=0; i<myBalloons.size(); i++){
//  myBalloons[i].update();
//    }
    
#ifdef _USE_LIVE_VIDEO
    vidGrabber.update();
    bNewFrame = vidGrabber.isFrameNew();
#else
    vidPlayer.update();
    bNewFrame = vidPlayer.isFrameNew();
#endif
    
    if (bNewFrame){
        
#ifdef _USE_LIVE_VIDEO
        colorImg.setFromPixels(vidGrabber.getPixels());
#else
        colorImg.setFromPixels(vidPlayer.getPixels());
#endif
        
        grayImage = colorImg;
        if (bLearnBakground == true){
            grayBg = grayImage;        // the = sign copys the pixels from grayImage into grayBg (operator overloading)
            bLearnBakground = false;
        }
        
        // take the abs value of the difference between background and incoming and then threshold:
        grayDiff.absDiff(grayBg, grayImage);
        grayDiff.threshold(threshold);
        
        // find contours which are between the size of 20 pixels and 1/3 the w*h pixels.
        // also, find holes is set to true so we will get interior contours as well....
        contourFinder.findContours(grayDiff, 20, (340*240)/3, 10, true);    // find holes
    }
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    // draw the incoming, the grayscale, the bg and the thresholded difference
    ofSetHexColor(0xffffff);
    colorImg.draw(20,20);
    grayImage.draw(360,20);
    grayBg.draw(20,280);
    grayDiff.draw(360,280);
    uint64_t frameIndex = 0;
    frameIndex = (int)(ofGetElapsedTimef() * 1) % 5;
    
    // then draw the contours:
    
    ofFill();
    ofSetHexColor(0x333333);
    ofDrawRectangle(360,540,320,240);
    ofSetHexColor(0xffffff);
    
    // we could draw the whole contour finder
    //contourFinder.draw(360,540);
    
    // or, instead we can draw each blob individually from the blobs vector,
    // this is how to get access to them:
    for (int i = 0; i < contourFinder.nBlobs; i++){
        contourFinder.blobs[i].draw(360,540);
        
        // draw over the centroid if the blob is a hole
        ofSetColor(255);
        if(contourFinder.blobs[i].hole){
            ofDrawBitmapString("hole",
                               contourFinder.blobs[i].boundingRect.getCenter().x + 360,
                               contourFinder.blobs[i].boundingRect.getCenter().y + 540);
        }
        if(contourFinder.blobs[i].hole){
           
//            for(int i =0; i<5;i++){
//                int t = i + 1 ;
//                 ofSetFrameRate(1) ;
                pic1.draw(0,0);
                pictures[frameIndex].draw(0,0);

//            }
        }
        ofSetFrameRate(60) ;
        
//        for (int i = 0 ; i<myBalloons.size(); i++) {
//            myBalloons[i].draw();
//        }
//        if(contourFinder.blobs[i].hole){
//            Balloon tempBalloons;                            // create the ball object
//            tempBalloons.setup(x,y, ofRandom(10,40));    // setup its initial state
//            myBalloons.push_back(tempBalloons);
        if(contourFinder.blobs[i].hole){
            for(int i =0; i<NUM_PARTICLES; i++){
                ofSetColor(color[i]);
                ofDrawCircle(position[i], radius[i]);
            }
    }
    }
    
    // finally, a report:
    ofSetHexColor(0xffffff);
    stringstream reportStr;
    reportStr << "bg subtraction and blob detection" << endl
    << "press ' ' to capture bg" << endl
    << "threshold " << threshold << " (press: +/-)" << endl
    << "num blobs found " << contourFinder.nBlobs << ", fps: " << ofGetFrameRate();
    ofDrawBitmapString(reportStr.str(), 20, 600);
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    switch (key){
        case ' ':
            bLearnBakground = true;
            break;
        case '+':
            threshold ++;
            if (threshold > 255) threshold = 255;
            break;
        case '-':
            threshold --;
            if (threshold < 0) threshold = 0;
            break;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
    
}
