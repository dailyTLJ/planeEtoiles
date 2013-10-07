#include "imageElement.h"

imageElement::imageElement() {

}

imageElement::imageElement(string filename) {
    this->loadImage(filename);
}

void imageElement::loadImage(string filename) {
    this->file = filename;
    img.loadImage(filename);
}

void imageElement::update() {

}

void imageElement::draw() {
    if (scale!=1.0) img.draw(position.x * scale, position.y * scale, w * scale, h * scale);
    else img.draw(position, w, h);

}

void imageElement::draw(int x, int y, float scale) {
//    cout << "draw \t x " << x << "   position.x " << position.x << "  scale " << scale << "   this->scale " << this->scale << endl;
    if (scale!=1.0) img.draw(x + position.x * scale, y + position.y * scale, w * scale, h * scale);
    else img.draw(x + position.x, y + position.y, w, h);
}

