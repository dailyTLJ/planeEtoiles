#include "videoElement.h"

videoElement::videoElement() {

}

videoElement::videoElement(string filename, float speed) {
    this->speed = speed;
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
}

void videoElement::reset() {
    // cout << "reset video " << endl;
    this->pause(false);
    movie.setSpeed(this->speed);
    movie.firstFrame();
}


void videoElement::draw() {
    if (scale!=1.0) movie.draw(position.x * scale, position.y * scale, w * scale, h * scale);
    else movie.draw(position, w, h);

}

void videoElement::draw(int x, int y, float scale) {
   // cout << "draw \t x " << x << "   position.x " << position.x << "  scale " << scale << "   this->scale " << this->scale << endl;
    if (scale!=1.0) movie.draw(x + position.x * scale, y + position.y * scale, w * scale, h * scale);
    else movie.draw(x + position.x, y + position.y, w, h);
}

