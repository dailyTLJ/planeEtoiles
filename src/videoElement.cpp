#include "videoElement.h"

videoElement::videoElement() {

}

videoElement::videoElement(string filename, float speed) {
    this->displaySpeed = speed;
    this->velocity.set(0,0);
    this->rotation = 0;
    this->moveElement = false;
    this->loadMovie(filename);
}

void videoElement::loadMovie(string filename) {
    this->file = filename;
    movie.loadMovie(filename);
    this->w = movie.getWidth();
    this->h = movie.getHeight();
//    cout << "video : loadMovie " << filename << "   (" << this->w << "|" << this->h << ")" << endl;
    this->play(true);
    this->pause(true);
}

void videoElement::play(bool loop) {
    movie.play();
    movie.setLoopState( loop ? OF_LOOP_NORMAL : OF_LOOP_NONE );
}

void videoElement::pause(bool v) {
    movie.setPaused(v);
}

void videoElement::update() {
    movie.update();
    if (this->moveElement) {
        position.x += velocity.x;
        position.y += velocity.y;
    }

    if (movie.getIsMovieDone()) {
            cout << "movie " << file << " ended,  destroy: " << this->selfdestroy << endl;
        if (this->selfdestroy) {
            this->dead = true;
        }
    }
}

void videoElement::reset() {
    // cout << "reset video " << endl;
    this->pause(false);
    movie.setSpeed(this->displaySpeed);
    movie.firstFrame();
}


void videoElement::draw() {
    ofPushMatrix();
    ofTranslate(position.x * scale, position.y * scale);
    ofRotateZ(this->rotation);
    if (scale!=1.0) movie.draw(0, 0, w * scale, h * scale);
    else movie.draw(0, 0, w, h);
    ofPopMatrix();
}

// being able to translate the drawing with the function
void videoElement::draw(int x, int y, float _scale) {
   // cout << "draw \t x " << x << "   position.x " << position.x << "  scale " << _scale << "   this->scale " << this->scale << endl;
    ofPushMatrix();
    ofTranslate(x + position.x * _scale, y + position.y * _scale);
    ofRotateZ(this->rotation);
    if (scale!=1.0) movie.draw(0, 0, w * scale * _scale, h * scale * _scale);
    else movie.draw(0, 0, w * _scale, h * _scale);
    ofPopMatrix();
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
        movie.setLoopState(OF_LOOP_NONE);
    } else {
        movie.setLoopState(OF_LOOP_NORMAL);
    }
}