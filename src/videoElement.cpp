#include "videoElement.h"

videoElement::videoElement() {
    this->hide = true;
    this->position.set(0,0);
    this->scale = 1.0;
}

void videoElement::loadMovie(string filename) {
    movie.loadMovie(filename);
    this->w = movie.getWidth();
    this->h = movie.getHeight();
}

void videoElement::play(bool loop) {
    movie.play();
    movie.setLoopState( loop ? OF_LOOP_NORMAL : OF_LOOP_NONE );
}

void videoElement::pause() {
    movie.setPaused(true);
}

void videoElement::update() {
    movie.update();
}


void videoElement::draw() {
    if (scale!=1.0) movie.draw(position, w * scale, h * scale);
    else movie.draw(position, w, h);
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
