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
        if (this->selfdestroy) {
            // cout << "movie " << file << " ended " << endl;
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
void videoElement::draw(int x, int y, float scale) {
   // cout << "draw \t x " << x << "   position.x " << position.x << "  scale " << scale << "   this->scale " << this->scale << endl;
    ofPushMatrix();
    ofTranslate(x + position.x * scale, y + position.y * scale);
    ofRotateZ(this->rotation);
    if (scale!=1.0) movie.draw(0, 0, w * scale, h * scale);
    else movie.draw(0, 0, w, h);
    ofPopMatrix();
}

// set at what speed a moving star should shoot across the screen, in all directions
void videoElement::moveAcross(float v, int maxw, int maxh, bool destr) {
    this->moveElement = true;
    rotation = ofRandom(-180, 180);
    int maxRadius = max(maxw/2, maxh/2);
    this->position.set( maxw/2 - maxRadius*1.5 * sin(ofDegToRad(rotation)), maxh/2 + maxRadius*1.5 * cos(ofDegToRad(rotation)) );
    this->velocity.set( v * sin(ofDegToRad(rotation)), -v * cos(ofDegToRad(rotation)) );
    this->selfdestroy = destr;
    rotation -= 90;
    if (destr) {
        movie.setLoopState(OF_LOOP_NONE);
    }
}

// set at what speed a moving star should shoot across the screen, top down
void videoElement::moveAcross(float vx, float vy, int maxw, bool destr) {
    this->moveElement = true;
    this->velocity.set(vx,vy);
    this->position.set( ofRandom(maxw), -500 - ofRandom(300) );
    this->selfdestroy = destr;
    rotation = 90 - ofRadToDeg(atan2(vx, vy));
    if (destr) {
        movie.setLoopState(OF_LOOP_NONE);
    }
}
