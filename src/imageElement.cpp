#include "imageElement.h"

imageElement::imageElement() {

}

imageElement::imageElement(string filename, float _scale) {
    this->loadImage(filename, _scale);
}

void imageElement::loadImage(string filename, float _scale) {
    this->file = filename;
    img.loadImage(filename);
    this->scale = _scale;
    this->w = img.getWidth() * _scale;
    this->h = img.getHeight() * _scale;
}

void imageElement::update() {
    mediaElement::update();
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
    ofSetColor(255, 255, 255, int(255*opacity));
    float msc = (scale+addSc) * _scale;
    if (centered) {
        img.draw(-w * msc * 0.5, -h * msc * 0.5, w * msc, h * msc);
    } else {
        img.draw(0, 0, w * msc, h * msc);
    }
    ofSetColor(255, 255, 255, 255);
}