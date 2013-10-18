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

        virtual void moveAcross(float v, int maxw, int maxh, bool destr);
        virtual void moveAcross(float vx, float vy, int maxw, bool destr);
        
        void setDisplay(int x, int y);
        void setDisplay(int x, int y, float _scale);
        void setDisplay(int x, int y, bool centered);
        void setDisplay(int x, int y, int w, int h);
        void setDisplay(int x, int y, int w, int h, bool centered);

        virtual void autoDestroy(bool v);
        virtual void loadMovie(string filename);

        virtual void reset();

        ofPoint position;
        int w;
        int h;
        int rotation;
        bool hide;
        float scale;
        string file;     

        bool selfdestroy;
        bool dead;

    protected:
    private:
};
