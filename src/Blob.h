#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"

#define MAX_HISTORY 100
#define NEIGHBOR_HISTORY 10

class TimedPoint {
    public:
        ofPoint point;
        int time;

        void set(int x, int y) {
            this->point.set( x, y );
            this->time = ofGetUnixTime();
        }
};

class Neighbor {
    public:
        int id;
        bool updated;
        vector<float> distance;
        bool sameDistance;
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
        float vel;

        vector<TimedPoint> rawHistory;
    	vector<TimedPoint> history;    // array length of MAX_HISTORY

        // internal information
        int lifetime;
        int maxLifetime;
        bool updated;	// updated by last sample ?

        // trail analysis
        bool frozen;
        int frozenStart;
        int frozenTimer;
        std::map<int, Neighbor> neighbors;


        Blob();
        void init();
        void follow(float x, float y);
        void setVelocity(float dx, float dy);
        void analyze(float freezeMinVel);
        void analyzeNeighbors(std::map<int, ofPoint> neighborLocation);
        ofPoint transformPerspective(ofPoint& v);
        void update();
        bool isAlive();


        cv::Mat* perspectiveMat;

};
