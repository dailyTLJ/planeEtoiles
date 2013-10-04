#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofEvents.h"
#include "videoElement.h"

#define MAX_HISTORY 100
#define NEIGHBOR_HISTORY 10
#define VELOCITY_HISTORY 10

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

//        Neighbor() { cout << "neighbor" << endl; };
//        ~Neighbor() { cout << "~neighbor" << endl; };
        int id;
        bool updated;
        vector<float> distance;
        bool steadyDistance;

        float getMean() {
            float sum = 0;
            for (std::vector<float>::iterator it = distance.begin(); it != distance.end(); ++it) {
                sum += *it;
            }
            return sum / distance.size();
        }

        float getVariance() {
            float mean = getMean();
            float tmp = 0;
            for (std::vector<float>::iterator it = distance.begin(); it != distance.end(); ++it) {
                tmp += (mean - *it) * (mean - *it);
            }
            return tmp / distance.size();
        }

        float getStdDev() {
            return sqrtf( getVariance() );
        }

        float getMedian() {
            std::vector<float> distArray = std::vector<float>(distance);
            std::sort(distArray.begin(), distArray.end());
            if (distArray.size() % 2 == 0) {
                return (distArray[(distArray.size()/2)-1] + distArray[distArray.size()/2] ) / 2.0;
            } else {
                return distArray[distArray.size()/2];
            }
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
        float vel;
        vector<float> velHistory;

        vector<TimedPoint> rawHistory;
    	vector<TimedPoint> history;    // array length of MAX_HISTORY

        // internal information
        int lifetime;
        int maxLifetime;
        bool updated;	// updated by last sample ?

        // trail analysis
        bool frozen;            //
        bool properFreeze;      // after a threshold of time
        int frozenStart;
        int frozenTimer;
        ofEvent<int> onFreeze;
        ofEvent<int> unFreeze;
        ofEvent<int> prepareToDie;

        bool movingMean;
        std::map<int, Neighbor> neighbors;

        bool onEdge;    // if blob disappears, most likely bc it escaped the frame


        Blob();
        ~Blob();
        void init();
        void follow(float x, float y, float frameW = 800, float frameH = 600, float margin = 0);
        void setVelocity(float dx, float dy);
        void analyze(float freezeMinVel, float freezeMinTime, float movingThr);
        void analyzeNeighbors(std::map<int, ofPoint> neighborLocation, float keepDistanceThr);
        ofPoint transformPerspective(ofPoint& v);
        void update();
        bool isAlive();

        cv::Mat* perspectiveMat;

        ofPtr<videoElement> videoLink;

};
