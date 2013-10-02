#include "videoElement.h"

videoElement::videoElement() {
    this->hide = true;
    this->position.set(0,0);
    this->scale = 1.0;
    this->speed = 1.0;
}

videoElement::videoElement(string filename, float speed) {
    this->hide = true;
    this->position.set(0,0);
    this->scale = 1.0;
    this->speed = speed;
    this->loadMovie(filename);
    this->file = filename;
}

void videoElement::loadMovie(string filename) {
    this->file = filename;
    movie.loadMovie(filename);
    this->w = movie.getWidth();
    this->h = movie.getHeight();
    cout << "video : loadMovie " << filename << "   (" << this->w << "|" << this->h << ")" << endl;
//    this->play(true);
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
    // // cout << "video update " << this->file << "  frame " << movie.getCurrentFrame() << endl;
    // if (ofGetFrameNum() % 2 == 0) movie.nextFrame();
    // if (movie.getCurrentFrame() == movie.getTotalNumFrames()) {
    //     // cout << "done" << endl;
    //     movie.setFrame(0);
    //     // movie.nextFrame();
    // }
    movie.update();
}

void videoElement::reset() {
//    this->pause(false);
//    movie.setSpeed(this->speed);
//    movie.firstFrame();
}


void videoElement::draw() {
    if (scale!=1.0) movie.draw(position, w * scale, h * scale);
    else movie.draw(position, w, h);

}

void videoElement::draw(int x, int y, float scale) {
    if (scale!=1.0) movie.draw(x, y, w * scale, h * scale);
    else movie.draw(x, y, w, h);
}


void videoElement::setDisplay(int x, int y, float scale) {
    this->position.set(x, y);
    this->scale = scale;
}

void videoElement::setDisplay(int x, int y, int w, int h) {
    this->position.set(x, y);
    this->w = w;
    this->h = h;
    this->scale = 1.0;
}
