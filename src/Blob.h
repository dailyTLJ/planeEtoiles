#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"

#define MAX_HISTORY 100

class TimedPoint {
    public:
        ofPoint point;
        int time;

        void set(int x, int y) {
            this->point.set( x, y );
            this->time = ofGetUnixTime();
        }
};

class Blob {
    public:
        int id;

        // information coming from blobserver
        // ofVec2f
    	ofPoint _rawPos;
    	ofPoint position;
    	ofPoint velocity;
        float size;
        float age;
        float lostDuration;

    	vector<TimedPoint> history;    // array length of MAX_HISTORY

        // internal information
        int lifetime;
        int maxLifetime;
        bool updated;	// updated by last sample ?

        // trail analysis


        Blob();
        void init();
        void follow(int x, int y);
        ofPoint transformPerspective(ofPoint& v);
        void update();
        bool isAlive();

        CvMat* perspectiveMat;

};
