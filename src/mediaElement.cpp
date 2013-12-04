#include "mediaElement.h"

mediaElement::mediaElement() {
    this->id = -1;
    this->hide = false;
    this->position.set(0,0);
    this->scale = 1.0;
    this->addSc = 0.0;
    this->selfdestroy = false;
    this->movieEndTrigger = false;
    this->loopFile = "";
    this->fadeoutDestroy = false;
    this->dead = false;
    this->clr = ofColor(255, 255, 255);
    this->opacityChange = 0.01f;
    this->opacity = 1.f;
    this->fading = false;
    this->scaling = false;
    this->centered = false;
    this->moveElement = false;
    this->velocity.set(0,0);
    goal.set(0,0);
    goalDefined = false;
    blend = true;
    visible = false;
    moveSpeed = 1.f;
    bridgeVideo = false;
    bridgeBlobID[0] = 0;
    bridgeBlobID[1] = 0;
    bridgeUpdated = false;
    outroTransformation = &mediaElement::fadeOut; 
    introTransformation = &mediaElement::fadeIn;
}


void mediaElement::update() {
    bridgeUpdated = false;
    if (moveElement) {
        position.x += velocity.x;
        position.y += velocity.y;
        if (opacity<1) opacity += opacityChange;
        if (goalDefined) {
            float distance = ofDist(position.x, position.y, goal.x, goal.y);
            if (distance < moveSpeed) {
                // cout << "mediaElement::update()  arrived at goal" << endl;
                moveElement = false;
                goalDefined = false;
                visible = true;
                position.set(goal.x, goal.y);
            } else {
                // velocity = goal - velocity;
                float rot = atan2(goal.x-position.x,goal.y-position.y);
                velocity.set(moveSpeed*sin(rot), moveSpeed*cos(rot));
                // cout << "adjust velocity   rot: " << rot << " goal: " << goal.x << "/" << goal.y << "   vel: " << velocity.x << "/" << velocity.y << endl;
            }
        }
    }
    if (addSc>0) {
        addSc-=0.01;
        if (addSc<0) addSc = 0;
    }
    if (fading) {
        opacity += opacityChange;
        // ofLogNotice("mediaElement") << "update opacity > " << opacity;
        if (opacityChange>0 && opacity >= 1.0f) {
            opacity = 1.f;
            fading = false;
            visible = true;
            ofNotifyEvent(fadeInEnd,this->w,this);
        } else if (opacityChange<0 && opacity <= 0.f) {
            opacity = 0.f;
            fading = false;
            visible = false;
            ofLogNotice("mediaElement") << ofGetFrameNum() << "\t" << "mediaElement::update opacity 0, fadeoutend";
            ofNotifyEvent(fadeOutEnd,this->w,this);
            if (fadeoutDestroy) dead = true;
        }
    }
    if (scaling) {
        scale *= 0.95;
        if (scale < 0.05) {
            visible = false;
            ofNotifyEvent(fadeOutEnd,this->w,this);
        }
    }
}

void mediaElement::draw() {

}

void mediaElement::bounce() {
    addSc += 0.1;
}

void mediaElement::bridge(int id1, int id2) {
    bridgeVideo = true;
    bridgeBlobID[0] = id1;
    bridgeBlobID[1] = id2;
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

void mediaElement::reset(bool vis) {
    hide = false;
    opacity = (vis) ? 1.f : 0.f;
    visible = (vis) ? true : false;
    opacityChange = 0.01f;
    fading = false;
}

void mediaElement::moveAcross(float vx, float vy, int maxw, int maxh, bool destr) {

}

void mediaElement::moveAcross(float vx, float vy, int maxw, bool destr) {

}

void mediaElement::finishMovie(float _speed) {
    fadeOut();
}

void mediaElement::finishMovie() {
    fadeOut();
}

void mediaElement::scaleAway() {
    if (hide) scale = 0.01;
    scaling = true;
}

void mediaElement::moveInFromTop() {
    goalDefined = true;
    moveElement = true;
    goal.set(position.x,position.y);
    position.y -= 1200;
    // velocity.set(0,2);
    opacity = 1.0f;
    moveSpeed = 3;
}

void mediaElement::moveInFromSide(int centerx, int centery) {
    goalDefined = true;
    moveElement = true;
    goal.set(position.x,position.y);
    float rot = atan2(position.x-centerx,position.y-centery);
    // if (rot==0) rot -= ofDegToRad(90);
    position.set(centerx + centery*sin(rot), centery + centery*cos(rot));
    // velocity.set(-2*sin(rot), -2*cos(rot));
    opacity = 0.f;
    opacityChange = 0.01; // don't do this with fade, to avoid triggering the fadeInEvent
    moveSpeed = 7;
    ofLogNotice("mediaElement") << ofGetFrameNum() << "\t" << " moveInFromSide   rot: " << rot << "  position: " << position.x << "/" << position.y << "  goal: " << goal.x << "/" << goal.y;
}

void mediaElement::fade(float speed) {
    fading = true;
    opacityChange = speed;
    // cout << "fade = true" << endl;
}

void mediaElement::fadeOut() {
    ofLogNotice("mediaElement") << ofGetFrameNum() << "\t" << "fadeOut()";
    if (hide) opacity = 0.01;       // set to low value, so there is at least one procession step
                                    // else endless feedback loop to event fgMediaFadedOut would be created
    fadeOut(0.01);
}

void mediaElement::fadeIn() {
    fadeIn(0.01);
}

void mediaElement::fadeOut(float speed, float op, bool destroy) {
    ofLogNotice("mediaElement") << ofGetFrameNum() << "\t" << file << "\tfadeOut()";
    opacity = op;
    fadeoutDestroy = destroy;
    fade(-speed);
}

void mediaElement::fadeIn(float speed) {
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