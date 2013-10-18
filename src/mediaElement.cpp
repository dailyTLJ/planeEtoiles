#include "mediaElement.h"

mediaElement::mediaElement() {
    this->hide = false;
    this->position.set(0,0);
    this->scale = 1.0;
    this->selfdestroy = false;
    this->dead = false;
}


void mediaElement::update() {

}

void mediaElement::draw() {

}

void mediaElement::draw(int x, int y, float _scale) {
    // cout << " draw at " << x << " px " << position.x << "  scale " << scale << "  width " << this->w << endl;
    if (!hide) {
        ofPushMatrix();
        ofTranslate(x + position.x * _scale + w*_scale*0.5, y + position.y * _scale + h*_scale*0.5);
        ofFill(); ofSetColor(155, 70, 0);
        ofCircle(0, 0, this->w * _scale * 0.5);
        ofPopMatrix();
    }
}

void mediaElement::reset() {

}

void mediaElement::moveAcross(float v, int maxw, int maxh, bool destr) {

}

void mediaElement::moveAcross(float vx, float vy, int maxw, bool destr) {

}



void mediaElement::setDisplay(int x, int y) {
    this->setDisplay(x, y, 1.0f);
}

void mediaElement::setDisplay(int x, int y, bool centered) {
    if (centered) {
        this->position.set( x - this->w/2, y - this->h/2 );
        this->scale = 1.0f;
    } else {
        this->setDisplay(x, y, 1.0f);
    }
}

void mediaElement::setDisplay(int x, int y, float _scale) {
//    cout << "setDisplay " << x << " | " << y << "   * " << scale << endl;
    this->position.set(x, y);
    this->scale = _scale;
}

void mediaElement::setDisplay(int x, int y, int w, int h) {
    this->position.set(x, y);
    this->w = w;
    this->h = h;
    this->scale = 1.0;
}

void mediaElement::setDisplay(int x, int y, int w, int h, bool centered) {
    // cout << "setDisplay " << x << " | " << y << endl;
    this->w = w;
    this->h = h;
    if (centered) {
        this->position.set( x-w/2, y-h/2 );
    } else {
        this->position.set(x, y);
    }
    this->scale = 1.0;
}

void mediaElement::autoDestroy(bool v) {

}


void mediaElement::loadMovie(string filename) {

}