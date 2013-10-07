#pragma once

#include "ofMain.h"
#include "mediaElement.h"

class imageElement: public mediaElement
{
    public:
        imageElement();
        imageElement(string filename);
        // ~imageElement() { cout << "kill image " << this->file << endl; };

        void update();

        void loadImage(string filename);
        
        void draw();
        void draw(int x, int y, float scale);

        ofImage img;

    protected:
    private:
};
