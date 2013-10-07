#pragma once

#include "ofMain.h"
#include "mediaElement.h"

class videoElement: public mediaElement
{
    public:
        videoElement();
        videoElement(string filename, float speed = 1.0);
        // ~videoElement() { cout << "kill video " << this->file << endl; };

        void update();

        void loadMovie(string filename);
        void play(bool loop = true);
        void pause(bool v = true);
        void reset();
        
        void draw();
        void draw(int x, int y, float scale);

        ofVideoPlayer movie;
        float speed;

    protected:
    private:
};
