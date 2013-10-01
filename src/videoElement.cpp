#include "videoElement.h"

videoElement::videoElement() {
    this->hide = true;
    this->position.set(0,0);
    this->scale = 1.0;
}

videoElement::videoElement(string filename) {
    this->hide = true;
    this->position.set(0,0);
    this->scale = 1.0;
    this->loadMovie(filename);
}

void videoElement::loadMovie(string filename) {
    movie.loadMovie(filename);
    this->w = movie.getWidth();
    this->h = movie.getHeight();
    cout << "video : loadMovie " << filename << "   (" << this->w << "|" << this->h << ")" << endl;
    movie.play();
}

void videoElement::play(bool loop) {
    movie.play();
    movie.setLoopState( loop ? OF_LOOP_NORMAL : OF_LOOP_NONE );
}

void videoElement::pause() {
    movie.setPaused(true);
}

void videoElement::update() {
//    cout << "video update" << endl;
    movie.update();
}


void videoElement::draw() {
    if (scale!=1.0) movie.draw(position, w * scale, h * scale);
    else movie.draw(position, w, h);

}

void videoElement::draw(int x, int y, float scale) {
//    cout << "video draw" << endl;
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
