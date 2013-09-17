#pragma once

#include "ofMain.h"

#define MAX_HISTORY 100

class TimedPoint {
    public:
        ofVec2f point;
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
    	ofVec2f _rawPos;
    	ofVec2f position;
    	ofVec2f velocity;
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
        ofVec2f transformPerspective(ofVec2f& v);
        void update();
        bool isAlive();

};
