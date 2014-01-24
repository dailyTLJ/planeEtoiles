#include "imageElement.h"

imageElement::imageElement() {

}

imageElement::imageElement(string filename, float _scale) {
    this->loadImage(filename, _scale);
}

void imageElement::loadImage(string filename, float _scale) {
    this->file = filename;
    ofFile fileTest(ofToDataPath(file));
    if (fileTest.exists()) {
        img.loadImage(filename);
        mediaLoaded = true;
    } else {
        mediaLoaded = false;
        ofLogError() << ofGetFrameNum() << "\t" << "file doesn't exist " << file;
    }
    this->scale = _scale;
    if (mediaLoaded) {
        this->w = img.getWidth() * _scale;
        this->h = img.getHeight() * _scale;
    }
}

void imageElement::update(float updateRate) {
    mediaElement::update(updateRate);
}

void imageElement::draw() {
    ofPushMatrix();
    ofTranslate(position.x, position.y);
    ofRotateZ(this->rotation);
    drawElement(1.0f);
    ofPopMatrix();
}

void imageElement::draw(int x, int y, float _scale) {
    ofPushMatrix();
    ofTranslate(x + position.x * _scale, y + position.y * _scale );
    ofRotateZ(this->rotation);
    drawElement(_scale);
    ofPopMatrix();
}

void imageElement::drawElement(float _scale) {
    if (mediaLoaded) {
        ofSetColor(255, 255, 255, int(255*opacity*opMax));
        float msc = (scale+addSc) * _scale;
        if (centered) {
            img.draw(-w * msc * 0.5, -h * msc * 0.5, w * msc, h * msc);
        } else {
            img.draw(0, 0, w * msc, h * msc);
        }
        ofSetColor(255, 255, 255, 255);
    }
}