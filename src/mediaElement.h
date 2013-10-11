#pragma once

#include "ofMain.h"
#include "ofEvents.h"

class mediaElement
{
    public:
        mediaElement();
        // ~mediaElement();

        virtual void update();

        virtual void draw();
        virtual void draw(int x, int y, float scale);

        virtual void moveAcross(float v, int maxw, int maxh, bool destr);
        virtual void moveAcross(float vx, float vy, int maxw, bool destr);
        
        void setDisplay(int x, int y);
        void setDisplay(int x, int y, float scale);
        void setDisplay(int x, int y, bool centered);
        void setDisplay(int x, int y, int w, int h);
        void setDisplay(int x, int y, int w, int h, bool centered);

        virtual void reset();

        ofPoint position;
        int w;
        int h;
        bool hide;
        float scale;
        string file;     

        bool selfdestroy;
        bool dead;

    protected:
    private:
};