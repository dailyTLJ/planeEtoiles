#include "videoElement.h"

videoElement::videoElement() {
    movie = ofPtr<ofVideoPlayer>( new ofVideoPlayer() );
}

videoElement::videoElement(string filename, bool _blend) {
    movie = ofPtr<ofVideoPlayer>( new ofVideoPlayer() );
    movie->setPixelFormat(OF_PIXELS_BGRA);  // need to set this even for non-alpha videos, 
                                            // because of alpha-fix in ofGstVideoPlayer.cpp
    this->blend = _blend;
    this->rotation = 0;
    this->setFileDeadNow = false;
    this->loadLoopFileNow = false;
    this->mediaLoaded = false;
    this->loadMovie(filename);
}

videoElement::~videoElement() {
    // cout << "~videoElement() " << endl;
    movie = ofPtr<ofVideoPlayer>( new ofVideoPlayer() );
    ofLogNotice("videoElement") << ofGetFrameNum() << "\t" << "destructor " << file;
}

void videoElement::loadMovie(string filename) {
    this->file = filename;
    ofLogNotice("videoElement") << ofGetFrameNum() << "\t" << "loadMovie\t'" << file << "'";
    mediaLoaded = movie->loadMovie(filename);
    if (mediaLoaded) {
        this->w = movie->getWidth();
        this->h = movie->getHeight();
        this->play(true);
        this->pause(true);
    } else {
        ofLogError() << ofGetFrameNum() << "\t" << "file doesn't exist " << file;
    }
}

void videoElement::play(bool loop) {
    if (mediaLoaded) {
        movie->play();
        movie->setLoopState( loop ? OF_LOOP_NORMAL : OF_LOOP_NONE );
        // ofLogNotice("videoElement") << ofGetFrameNum() << "\t" << "play  setLoopState " << loop;
    }
}

void videoElement::pause(bool v) {
    if (mediaLoaded) {
        movie->setPaused(v);
    }
}

void videoElement::update() {
    if (mediaLoaded) {
        mediaElement::update();
        if (loadLoopFileNow) {
            ofLogNotice("videoElement") << ofGetFrameNum() << "\t" << "loadLoopFileNow\t" << loopFile;
            loadLoopFileNow = false;
            loadMovie(loopFile);
            loopFile = "";
            play(true);
        }
        if (setFileDeadNow) {
            setFileDeadNow = false;
            this->dead = true;
        }

        movie->update();
        
        if (movie->getIsMovieDone()) {
            // cout << "movie " << file << " ended,  destroy: " << this->selfdestroy << endl;
            if (this->selfdestroy) {
                play(true);
                setFileDeadNow = true;
                // this->dead = true;
                if (this->movieEndTrigger) {
                    ofLogNotice("videoElement") << ofGetFrameNum() << "\t" << "movieisdone, selfdestroy trigger\t" << file;
                    ofNotifyEvent(fadeOutEnd,this->w,this);
                } else {
                    ofLogNotice("videoElement") << ofGetFrameNum() << "\t" << "movieisdone, selfdestroy\t" << file;
                    movie->update();
                }
            } else if (this->movieEndTrigger && loopFile!="") {
                play(true);
                movie->update();
                ofLogNotice("videoElement") << ofGetFrameNum() << "\t" << "movieisdone, loopfile\t" << loopFile;
                loadLoopFileNow = true;
                // loadMovie(loopFile);
                // loopFile = "";
                // play(true);
            } else if (this->movieEndTrigger) {
                play(true); // when set to unpaused, it won't trigger the GStreamer Critical unref error?
                ofLogNotice("videoElement") << ofGetFrameNum() << "\t" << "movieisdone, fadeoutend\t" << file;
                ofNotifyEvent(fadeOutEnd,this->w,this);
                // this->dead = true;
            }
        }
    }
}

void videoElement::reset(bool visible) {
    if (mediaLoaded) {
        // ofLogNotice("videoElement") << ofGetFrameNum() << "\t" << "reset video " << file << "  " << visible;
        mediaElement::reset(visible);
        displaySpeed = 1.f;
        this->pause(false);
        movie->setSpeed(this->displaySpeed);
        movie->firstFrame();
        // movie->update();
        // movie->nextFrame();
    }
}

void videoElement::finishMovie() {
    finishMovie(5.0);
}

void videoElement::finishMovie(float _speed) {
    if (mediaLoaded) {
        // end transformation for 0-IDLE video = play fast to end
        ofLogNotice("videoElement") << ofGetFrameNum() << "\t" << "finishMovie()\t" << file ;
        // if (hide) 
        movieEndTrigger = true;
        movie->setLoopState(OF_LOOP_NONE);
        this->displaySpeed = _speed;
        movie->setSpeed(this->displaySpeed);
    }
}

void videoElement::draw() {
    if (!hide) {
        ofPushMatrix();
        ofTranslate(position.x, position.y);
        ofRotateZ(this->rotation);
        drawElement(1.0f);
        ofPopMatrix();
    }
}

// being able to translate the drawing with the function
void videoElement::draw(int x, int y, float _scale) {
    if (!hide) {
       // cout << "draw \t x " << x << "   position.x " << position.x << "  scale " << _scale << "   this->scale " << this->scale << endl;
        ofPushMatrix();
        ofTranslate(x + position.x * _scale, y + position.y * _scale);
        ofRotateZ(this->rotation);
        drawElement(_scale);
        ofPopMatrix();
    }
}

void videoElement::drawElement(float _scale) {
    if (mediaLoaded) {
        ofSetColor(255, 255, 255, int(255*opacity*opMax));
        float msc = (scale+addSc) * _scale;
        if (centered) {
            movie->draw(-w * msc * 0.5, -h * msc * 0.5, w * msc, h * msc);
        } else {
            movie->draw(0, 0, w * msc, h * msc);
        }
        ofSetColor(255, 255, 255, 255);
    }
}

// set at what speed a moving star should shoot across the screen, in all directions
void videoElement::moveAcross(float vx, float vy, int maxw, int maxh, bool destr) {
    this->moveElement = true;
    rotation = ofRadToDeg(atan2(vx, vy));
    ofLogNotice("videoElement") << ofGetFrameNum() << "\t" << "moveAcross()\t" << rotation ;
    // limit rotation to  45-135  and  225-315
    // so that shooting stars only fly left---->right or left<----right
    if (rotation < -135) rotation = -135;
    else if (rotation > -45 && rotation <0) rotation = -45;
    else if (rotation < 45 && rotation >=0) rotation = 45;
    else if (rotation > 135 && rotation < 180) rotation = 135;
    else if (rotation >= 180 && rotation <225) rotation = 225;
    else if (rotation > 315) rotation = 315;


    ofLogNotice("videoElement") << ofGetFrameNum() << "\t" << "moveAcross()\t >>> " << rotation ;
    float v = 45;
    int maxRadius = max(maxw/2, maxh/2);
    this->position.set( maxw/2 - maxRadius*1.5 * sin(ofDegToRad(rotation)), maxh/2 + maxRadius*1.5 * cos(ofDegToRad(rotation)) );
    // slightly random, to avoid that all are flying perfectly through the center point of the screen
    rotation += ofRandom(-7,7);
    this->velocity.set( v * sin(ofDegToRad(rotation)), -v * cos(ofDegToRad(rotation)) );
    rotation -= 90;
    // autoDestroy(destr);
    this->selfdestroy = destr;
}

// set at what speed a moving star should shoot across the screen, top down
void videoElement::moveAcross(float vx, float vy, int maxw, bool destr) {
    this->moveElement = true;
    this->velocity.set(vx,vy);
    this->position.set( ofRandom(maxw), -500 - ofRandom(300) );
    rotation = 90 - ofRadToDeg(atan2(vx, vy));
    // autoDestroy(destr);
    this->selfdestroy = destr;
}


void videoElement::autoDestroy(bool v) {
    if (mediaLoaded) {
        this->selfdestroy = v;
        if (v) {
            movie->setLoopState(OF_LOOP_NONE);
        } else {
            movie->setLoopState(OF_LOOP_NORMAL);
        }
    }
}