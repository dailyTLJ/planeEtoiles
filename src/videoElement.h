#pragma once

#include "ofMain.h"

class videoElement
{
    public:
        videoElement();
        void update();
        void loadMovie(string filename);
        void play(bool loop = true);
        void pause();
        void draw();
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
