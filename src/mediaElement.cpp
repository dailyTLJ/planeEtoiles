#include "mediaElement.h"

mediaElement::mediaElement() {
    this->hide = false;
    this->position.set(0,0);
    this->scale = 1.0;
    this->selfdestroy = false;
    this->dead = false;
    this->clr = ofColor(255, 255, 255);
    this->opacityChange = 0.f;
    this->opacity = 1.f;
    this->fading = false;
}


void mediaElement::update() {
    if (fading) {
        opacity += opacityChange;
        // cout << "update opacity > " << opacity << endl;
        if (opacityChange>0 && opacity >= 1.0f) {
            opacity = 1.f;
        } else if (opacityChange<0 && opacity <= 0.f) {
            opacity = 0.f;
            fading = false;
            ofNotifyEvent(transitionEnd,this->w,this);
        }
    }
}

void mediaElement::draw() {

}

void mediaElement::draw(int x, int y, float _scale) {
    // cout << " draw at " << x << " px " << position.x << "  scale " << scale << "  width " << this->w << endl;
    if (!hide) {
        ofPushMatrix();
        ofTranslate(x + position.x * _scale + w*_scale*0.5, y + position.y * _scale + h*_scale*0.5);
        ofFill(); ofSetColor(clr);
        ofCircle(0, 0, this->w * _scale * 0.5);
        ofPopMatrix();
    }
}

void mediaElement::drawElement(float _scale) {

}

void mediaElement::reset() {

}

void mediaElement::moveAcross(float v, int maxw, int maxh, bool destr) {

}

void mediaElement::moveAcross(float vx, float vy, int maxw, bool destr) {

}

void mediaElement::endTransformation() {
    
}

void mediaElement::fade(float speed) {
    fading = true;
    opacityChange = speed;
    // cout << "fade = true" << endl;
}

void mediaElement::fadeOut(float speed) {
    cout << "mediaElement::fadeOut" << endl;
    fade(-speed);
}

void mediaElement::fadeIn(float speed) {
    opacity = 0.0f;
    fade(speed);
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