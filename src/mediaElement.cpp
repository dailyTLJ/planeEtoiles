#include "mediaElement.h"

mediaElement::mediaElement() {
    this->hide = false;
    this->position.set(0,0);
    this->scale = 1.0;
    this->addSc = 0.0;
    this->selfdestroy = false;
    this->dead = false;
    this->clr = ofColor(255, 255, 255);
    this->opacityChange = 0.01f;
    this->opacity = 1.f;
    this->fading = false;
    this->scaling = false;
    this->centered = false;
    this->moveElement = false;
    this->velocity.set(0,0);
    this->goal.set(0,0);
    this->goalDefined = false;
    this->blend = true;
    outroTransformation = &mediaElement::fadeOut; 
    introTransformation = &mediaElement::fadeIn;
}


void mediaElement::update() {
    if (moveElement) {
        position.x += velocity.x;
        position.y += velocity.y;
        if (goalDefined) {
            float distance = ofDist(position.x, position.y, goal.x, goal.y);
            if (distance < 2) {
                moveElement = false;
                goalDefined = false;
                position.set(goal.x, goal.y);
            }
        }
    }
    if (addSc>0) {
        addSc-=0.01;
        if (addSc<0) addSc = 0;
    }
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

void mediaElement::bounce() {
    addSc += 0.1;
}

void mediaElement::draw(int x, int y, float _scale) {
    // cout << " draw at " << x << " px " << position.x << "  scale " << scale << "  width " << this->w << endl;
    if (!hide) {
        ofPushMatrix();
        ofTranslate(x + position.x * _scale, y + position.y * _scale );
        ofFill(); ofSetColor(clr.r, clr.g, clr.b, int(255*opacity));
        ofCircle(0, 0, this->w * (scale+addSc) * _scale * 0.5);
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

void mediaElement::moveInFromTop() {
    cout << "mediaElement::moveInFromTop" << endl;
    goalDefined = true;
    moveElement = true;
    goal.set(position.x,position.y);
    position.y -= 1200;
    velocity.set(0,2);
    opacity = 1.0f;
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

void mediaElement::fadeIn() {
    fadeIn(0.01);
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
    centered = false;
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