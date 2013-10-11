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

}

void imageElement::draw() {
    if (scale!=1.0) img.draw(position.x * scale, position.y * scale, w * scale, h * scale);
    else img.draw(position, w, h);

}

void imageElement::draw(int x, int y, float _scale) {
//    cout << "draw \t x " << x << "   position.x " << position.x << "  scale " << scale << "   this->scale " << this->scale << endl;
    if (scale!=1.0) img.draw(x + position.x * _scale, y + position.y * _scale, w * _scale * scale, h * _scale * scale);
    else img.draw(x + position.x * _scale, y + position.y * _scale, w * _scale, h * _scale);
}
