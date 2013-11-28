#pragma once

#include "ofMain.h"
#include "mediaElement.h"

class videoElement: public mediaElement
{
    public:
        videoElement();
        videoElement(string filename, float speed = 1.0);
        ~videoElement();

        void update();

        void loadMovie(string filename);
        void play(bool loop = true);
        void pause(bool v = true);
        void reset(bool visible = true);

        void draw();
        void draw(int x, int y, float _scale);
        void drawElement(float _scale);

        void moveAcross(float vx, float vy, int maxw, int maxh, bool destr);
        void moveAcross(float vx, float vy, int maxw, bool destr);

        void autoDestroy(bool v);
        void finishFast();

        bool endFade;

        
        float displaySpeed;

        


    protected:
    private:
};
