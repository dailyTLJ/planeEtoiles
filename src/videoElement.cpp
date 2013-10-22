#include "videoElement.h"

videoElement::videoElement() {
    movie = ofPtr<ofVideoPlayer>( new ofVideoPlayer() );
}

videoElement::videoElement(string filename, float speed) {
    movie = ofPtr<ofVideoPlayer>( new ofVideoPlayer() );
    this->displaySpeed = speed;
    this->endFade = false;
    this->velocity.set(0,0);
    this->rotation = 0;
    this->moveElement = false;
    this->loadMovie(filename);
}

videoElement::~videoElement() {
    // cout << "~videoElement() " << endl;
    // cout << "~videoElement() : kill video " << this->file << endl;
}

void videoElement::loadMovie(string filename) {
    // cout << "video : loadMovie " << filename << endl;
    this->file = filename;
    movie->loadMovie(filename);
    this->w = movie->getWidth();
    this->h = movie->getHeight();
    this->play(true);
    this->pause(true);
}

void videoElement::play(bool loop) {
    movie->play();
    movie->setLoopState( loop ? OF_LOOP_NORMAL : OF_LOOP_NONE );
}

void videoElement::pause(bool v) {
    movie->setPaused(v);
}

void videoElement::update() {
    mediaElement::update();
    movie->update();
    if (this->moveElement) {
        position.x += velocity.x;
        position.y += velocity.y;
    }

    if (movie->getIsMovieDone()) {
        // cout << "movie " << file << " ended,  destroy: " << this->selfdestroy << endl;
        if (this->selfdestroy) {
            this->dead = true;
        }
        if (this->endFade && !fading) {
            fadeOut();
        }
    }
}

void videoElement::reset(bool visible) {
    cout << "reset video " << file << "  " << visible << endl;
    mediaElement::reset(visible);
    displaySpeed = 1.f;
    this->pause(false);
    movie->setSpeed(this->displaySpeed);
    movie->firstFrame();
}

void videoElement::finishFast() {
    // end transformation for 0-IDLE video = play fast to end
    cout << "videoElement::finishFast" << endl;
    // if (hide) 
    endFade = true;
    movie->setLoopState(OF_LOOP_NONE);
    this->displaySpeed = 5.0f;
    movie->setSpeed(this->displaySpeed);
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
    ofSetColor(255, 255, 255, int(255*opacity));
    if (centered) {
        movie->draw(-w * scale * _scale * 0.5, -h * scale * _scale * 0.5, w * scale * _scale, h * scale * _scale);
    } else {
        movie->draw(0, 0, w * scale * _scale, h * scale * _scale);
    }
    ofSetColor(255, 255, 255, 255);
}

// set at what speed a moving star should shoot across the screen, in all directions
void videoElement::moveAcross(float v, int maxw, int maxh, bool destr) {
    this->moveElement = true;
    rotation = ofRandom(-180, 180);
    int maxRadius = max(maxw/2, maxh/2);
    this->position.set( maxw/2 - maxRadius*1.5 * sin(ofDegToRad(rotation)), maxh/2 + maxRadius*1.5 * cos(ofDegToRad(rotation)) );
    this->velocity.set( v * sin(ofDegToRad(rotation)), -v * cos(ofDegToRad(rotation)) );
    rotation -= 90;
    autoDestroy(destr);
}

// set at what speed a moving star should shoot across the screen, top down
void videoElement::moveAcross(float vx, float vy, int maxw, bool destr) {
    this->moveElement = true;
    this->velocity.set(vx,vy);
    this->position.set( ofRandom(maxw), -500 - ofRandom(300) );
    rotation = 90 - ofRadToDeg(atan2(vx, vy));
    autoDestroy(destr);
}


void videoElement::autoDestroy(bool v) {
    this->selfdestroy = v;
    if (v) {
        movie->setLoopState(OF_LOOP_NONE);
    } else {
        movie->setLoopState(OF_LOOP_NORMAL);
    }
}