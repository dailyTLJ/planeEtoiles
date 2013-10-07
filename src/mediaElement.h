#pragma once

#include "ofMain.h"

class mediaElement
{
    public:
        mediaElement();
        // ~mediaElement();

        virtual void update();

        virtual void draw();
        virtual void draw(int x, int y, float scale);
        
        void setDisplay(int x, int y);
        void setDisplay(int x, int y, float scale);
        void setDisplay(int x, int y, bool centered);
        void setDisplay(int x, int y, int w, int h);

        virtual void reset();

        ofPoint position;
        int w;
        int h;
        bool hide;
        float scale;
        string file;

    protected:
    private:
};
