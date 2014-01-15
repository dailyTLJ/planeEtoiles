#pragma once

#include "ofMain.h"
#include "mediaElement.h"

class videoElement: public mediaElement
{
    public:
        videoElement();
        videoElement(string filename, bool _blend = true);
        ~videoElement();

        void update(float updateRate = 1.0);

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
        void finishMovie(float speed = 5.0);
        void finishMovie();

        float displaySpeed;

        bool loadLoopFileNow;
        bool setFileDeadNow;


    protected:
    private:
};
