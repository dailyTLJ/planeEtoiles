#pragma once

#include "ofMain.h"

class videoElement
{
    public:
        videoElement();
        videoElement(string filename, float speed = 1.0);
        ~videoElement() { cout << "kill video " << this->file << endl; };

        void update();
        void loadMovie(string filename);
        void play(bool loop = true);
        void pause(bool v = true);
        void reset();
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
        float speed;
        string file;


    protected:
    private:
};
