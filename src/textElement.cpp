#include "textElement.h"

textElement::textElement() {
    rawText = "";
    compText = "";
    textLineH = 40;
}

textElement::textElement(string t, string c) {
    rawText = "";
    compText = "";
    textLineH = 40;
    this->setText(t,c);
}

void textElement::setText(string t, string c) {
    this->rawText = t;
    this->compText = c;
    this->hide = false;

    textLine = ofSplitString(rawText, "\n");
    compLine = ofSplitString(compText, "\n");
}

void textElement::update(float updateRate) {
    mediaElement::update(updateRate);
}

void textElement::draw(ofTrueTypeFont * font) {
    if (!hide) {
        ofPushMatrix();
        ofTranslate(position.x, position.y);
        ofRotateZ(this->rotation);
        drawElement(font);
        ofPopMatrix();
    }
}

void textElement::draw(ofTrueTypeFont * font, int x, int y) {
    if (!hide) {
        ofPushMatrix();
        ofTranslate(x + position.x, y + position.y );
        ofRotateZ(this->rotation);
        drawElement(font);
        ofPopMatrix();
    }
}

void textElement::draw(int x, int y) {
    draw(mainFont, x, y);
}

void textElement::drawElement(ofTrueTypeFont * font) {
    ofSetColor(255, 255, 255, int(255*opacity*opMax));
    int i = 0;
    for (vector<string>::iterator it = textLine.begin() ; it != textLine.end(); ++it) {
        ofRectangle textR = font->getStringBoundingBox(compLine[i],0, 0);
        font->drawString((*it), -textR.width/2, i*font->getLineHeight());
        i++;
    }
}

void textElement::setFont(ofTrueTypeFont * font) {
    mainFont = font;
}