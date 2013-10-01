#pragma once

#include "ofMain.h"

class videoElement
{
    public:
        videoElement();
        videoElement(string filename);
        void update();
        void loadMovie(string filename);
        void play(bool loop = true);
        void pause();
        void draw();
        void draw(int x, int y, float scale);
        void setDisplay(int x, int y, float scale = 1.0);
        void setDisplay(int x, int y, int w, int h);

        ofVideoPlayer movie;
        ofPoint position;
        int w;
        int h;
        bool hide;
        float scale;


    protected:
    private:
};
