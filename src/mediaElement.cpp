#include "mediaElement.h"

mediaElement::mediaElement() {
    this->hide = true;
    this->position.set(0,0);
    this->scale = 1.0;
    this->selfdestroy = false;
    this->dead = false;
}


void mediaElement::update() {

}

void mediaElement::draw() {

}

void mediaElement::draw(int x, int y, float scale) {

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
//    cout << "setDisplay " << x << " | " << y << "   * " << scale << endl;
    if (centered) {
        this->position.set( x-this->w/2, y-this->h/2 );
        this->scale = 1.0f;
    } else {
        this->setDisplay(x, y, 1.0f);
    }
}

void mediaElement::setDisplay(int x, int y, float scale) {
//    cout << "setDisplay " << x << " | " << y << "   * " << scale << endl;
    this->position.set(x, y);
    this->scale = scale;
}

void mediaElement::setDisplay(int x, int y, int w, int h) {
    this->position.set(x, y);
    this->w = w;
    this->h = h;
    this->scale = 1.0;
}
