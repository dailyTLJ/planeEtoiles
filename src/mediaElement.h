#pragma once

#include "ofMain.h"
#include "ofEvents.h"

class mediaElement
{
    public:
        mediaElement();
        ~mediaElement();

        virtual void update(float updateRate = 1.0);
        virtual void draw();
        virtual void draw(int x, int y, float _scale);
        virtual void drawElement(float _scale);


        virtual void moveAcross(float vx, float vy, int maxw, int maxh, bool destr);
        virtual void moveAcross(float vx, float vy, int maxw, bool destr);
        virtual void autoDestroy(bool v);
        virtual void loadMovie(string filename);
        virtual void reset(bool vis = true);
        
        void setDisplay(int x, int y);
        void setDisplay(ofPoint p);
        void setDisplay(int x, int y, float _scale);
        void setDisplay(int x, int y, bool _centered);
        void setDisplay(ofPoint p, bool _centered);
        void setDisplay(int x, int y, int w, int h);
        void setDisplay(int x, int y, int w, int h, bool _centered);

        void fade(float speed);
        void fadeOut();
        void fadeIn();
        void fadeTo(float _opMax);
        void fadeOut(float speed, float op = 1.0, bool destroy = false);
        void fadeIn(float speed);

        void (mediaElement::*outroTransformation)();
        void (mediaElement::*introTransformation)();
        virtual void finishMovie();
        virtual void finishMovie(float _speed = 5.0);
        virtual void scaleAway();
        virtual void play(bool loop = true);
        void moveInFromTop();
        void moveInFromSide(int centerx, int centery);

        void bounce();

        int id;
        ofPoint position;
        int w;
        int h;
        bool centered;
        int rotation;
        bool hide;
        float scale;
        float addSc;
        string file; 
        string loopFile;
        ofColor clr;    
        bool fading;
        bool scaling;
        bool visible;
        float opacityChange;
        float opacity;
        float opMax;
        bool moveElement;
        float moveSpeed;
        ofPoint velocity;
        ofPoint goal;
        bool goalDefined;
        bool blend;

        bool selfdestroy;
        bool fadeoutDestroy;
        bool movieEndTrigger;
        bool dead;
        bool mediaLoaded;

        bool bridgeVideo;
        int bridgeBlobID[2];
        bool bridgeUpdated;
        void bridge(int id1, int id2);

        ofEvent<int> fadeOutEnd;
        ofEvent<int> fadeInEnd;
        ofEvent<int> playLoop;

        ofPtr<ofVideoPlayer> movie;
        bool fullyLoaded;

    protected:
    private:
};
