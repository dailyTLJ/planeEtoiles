#pragma once

#include "ofMain.h"

class mediaElement
{
    public:
        mediaElement();
        // ~mediaElement();

        void update();

        void draw();
        void draw(int x, int y, float scale);
        
        void setDisplay(int x, int y);
        void setDisplay(int x, int y, float scale);
        void setDisplay(int x, int y, bool centered);
        void setDisplay(int x, int y, int w, int h);

        ofPoint position;
        int w;
        int h;
        bool hide;
        float scale;
        string file;

    protected:
    private:
};
