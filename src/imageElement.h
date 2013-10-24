#pragma once

#include "ofMain.h"
#include "mediaElement.h"

class imageElement: public mediaElement
{
    public:
        imageElement();
        imageElement(string filename, float _scale = 1.0f);
        // ~imageElement() { cout << "kill image " << this->file << endl; };

        void update();

        void loadImage(string filename, float _scale = 1.0f);

        void draw();
        void draw(int x, int y, float _scale);
        void drawElement(float _scale);

        ofImage img;

    protected:
    private:
};
