#include "mediaElement.h"

mediaElement::mediaElement() {
    this->hide = false;
    this->position.set(0,0);
    this->scale = 1.0;
    this->selfdestroy = false;
    this->dead = false;
    this->clr = ofColor(255, 255, 255);
    this->opacityChange = 0.01f;
    this->opacity = 1.f;
    this->fading = false;
    this->scaling = false;
    this->centered = false;
    endTransformation = &mediaElement::fadeOut; 
}


void mediaElement::update() {
    if (fading) {
        opacity += opacityChange;
        // cout << "update opacity > " << opacity << endl;
        if (opacityChange>0 && opacity >= 1.0f) {
            opacity = 1.f;
            fading = false;
            ofNotifyEvent(fadeInEnd,this->w,this);
        } else if (opacityChange<0 && opacity <= 0.f) {
            opacity = 0.f;
            fading = false;
            cout << "mediaElement fadedout, call notifyEvent" << endl;
            ofNotifyEvent(fadeOutEnd,this->w,this);
        }
    }
    if (scaling) {
        scale *= 0.95;
        if (scale < 0.05) {
            cout << "mediaElement scaled away, call notifyEvent" << endl;
            ofNotifyEvent(fadeOutEnd,this->w,this);
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
        ofFill(); ofSetColor(clr.r, clr.g, clr.b, int(255*opacity));
        ofCircle(0, 0, this->w * scale * _scale * 0.5);
        ofPopMatrix();
    }
}

void mediaElement::drawElement(float _scale) {

}

void mediaElement::reset(bool visible) {
    opacity = (visible) ? 1.f : 0.f;
    opacityChange = 0.01f;
    fading = false;
}

void mediaElement::moveAcross(float v, int maxw, int maxh, bool destr) {

}

void mediaElement::moveAcross(float vx, float vy, int maxw, bool destr) {

}

void mediaElement::finishFast() {
    fadeOut();
}

void mediaElement::scaleAway() {
    cout << "mediaElement::scaleAway" << endl;
    if (hide) scale = 0.01;
    scaling = true;
}

void mediaElement::fade(float speed) {
    fading = true;
    opacityChange = speed;
    // cout << "fade = true" << endl;
}

void mediaElement::fadeOut() {
    if (hide) opacity = 0.01;       // set to low value, so there is at least one procession step
                                    // else endless feedback loop to event fgMediaFadedOut would be created
    fadeOut(0.01);
}

void mediaElement::fadeOut(float speed) {
    cout << "mediaElement::fadeOut  " << file << endl;
    fade(-speed);
}

void mediaElement::fadeIn(float speed) {
    cout << "mediaElement::fadeIn  " << file << endl;
    opacity = 0.0f;
    fade(speed);
}




void mediaElement::setDisplay(int x, int y) {
    this->setDisplay(x, y, 1.0f);
}

void mediaElement::setDisplay(int x, int y, bool _centered) {
    this->scale = 1.0f;
    this->setDisplay(x, y, 1.0f);
    this->centered = _centered;
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

void mediaElement::setDisplay(int x, int y, int w, int h, bool _centered) {
    // cout << "setDisplay " << x << " | " << y << endl;
    this->w = w;
    this->h = h;
    this->centered = _centered;
    this->position.set(x, y);
    this->scale = 1.0;
}

void mediaElement::autoDestroy(bool v) {

}


void mediaElement::loadMovie(string filename) {

}