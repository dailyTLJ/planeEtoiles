#pragma once

#include "ofMain.h"
#include "ofEvents.h"

class mediaElement
{
    public:
        mediaElement();
        ~mediaElement()  { /*cout << "kill media " << this->file << endl;*/ };

        virtual void update();
        virtual void draw();
        virtual void draw(int x, int y, float _scale);
        virtual void drawElement(float _scale);

        virtual void moveAcross(float v, int maxw, int maxh, bool destr);
        virtual void moveAcross(float vx, float vy, int maxw, bool destr);
        virtual void autoDestroy(bool v);
        virtual void loadMovie(string filename);
        virtual void reset(bool visible = true);
        // virtual void endTransformation();
        
        void setDisplay(int x, int y);
        void setDisplay(int x, int y, float _scale);
        void setDisplay(int x, int y, bool _centered);
        void setDisplay(int x, int y, int w, int h);
        void setDisplay(int x, int y, int w, int h, bool _centered);

        void fade(float speed);
        void fadeOut();
        void fadeOut(float speed);
        void fadeIn(float speed = 0.01);

        void (mediaElement::*endTransformation)();
        virtual void finishFast();
        virtual void scaleAway();


        ofPoint position;
        int w;
        int h;
        bool centered;
        int rotation;
        bool hide;
        float scale;
        string file; 
        ofColor clr;    
        bool fading;
        bool scaling;
        float opacityChange;
        float opacity;

        bool selfdestroy;
        bool dead;

        ofEvent<int> fadeOutEnd;
        ofEvent<int> fadeInEnd;

    protected:
    private:
};
