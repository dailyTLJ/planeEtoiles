#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "ofxOpenCv.h"
#include "ofxGui.h"
#include "ofEvents.h"

#include <map>
#include "Blob.h"
#include "videoElement.h"
#include "imageElement.h"

// listen to blobserver on port 9000
#define PORT 9000

class sceneInfo {
    public:
        string name;
        int no;
        int segments;
        int length[6];
        string instructions[6];
};

class planeApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
		void exit();

		void initScenes();
		void receiveOsc();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		void drawRawData(int x, int y, float scale);
		void drawTopDown(int x, int y, float scale, bool detailed = false);
		void drawScreen(int x, int y, float scale);
		void drawAnalysis(int x, int y, float scale);
		void drawControlInfo(int x, int y);
		void setPerspective();
		void recalculatePerspective(int & v);

		void blobOnFreeze(int & blobID);
		void blobUnFreeze(int & blobID);
		void blobOverFreeze(int & blobID);
		void blobUnlink(int & blobID);
		void blobOnCreate(int & blobID);
		void videoFollowBlob(int & blobID);
		void blobOnLost(int & blobID);
		void bgMediaFadedOut(int & transitionType);
		void bgMediaFadedIn(int & transitionType);

		void blobCountChange();
		void startTransition(int direction = 1);
		void nextSegment(int direction = 1);

		int projectionW;
        int projectionH;
        int blobserverW;
        int blobserverH;
        int blobW;
        int blobH;

		ofTrueTypeFont font;
		ofxPanel gui;
		ofxOscReceiver receiver;

		int mouseX, mouseY;
		string mouseButtonState;

		std::map<int, Blob> blobs;
		Blob testBlob;
		cv::Mat perspectiveMat;

		bool fullscreen;		 
        bool autoplay; 			 // advance to next segment by itself
		int scene;
		int segment;
		int segmentChange;
		int masterClock;
		int segmentClock;
		int segmentStart;
		int globalStart;
		bool moveOn;
		bool transition;
		bool success;			// if people followed the instructions
		bool flash;				// in between transitions, catch attention
		int flashCnt;
		int flashMax;

		std::map<int, sceneInfo> scenes;

		bool drawBlobDetail;

		ofParameter<float> freezeMinVel;
		ofParameter<int> freezeMinTime;
		ofParameter<int> freezeMaxTime;

		ofParameter<float> keepDistanceThr;
		ofParameter<float> movingThr;
		ofParameter<int> edgeMargin;
		ofParameter<int> hopLength;

		ofParameter<int> siteW;
		ofParameter<int> siteH;
		ofParameter<float> mapSiteW;
		ofParameter<float> mapSiteH;
		ofParameter<int> offsetX;
		ofParameter<int> offsetY;

		std::vector< ofPtr<mediaElement> > fgMedia;
//		std::map<int, std::vector<videoElement> > fgVideos;
		std::map<int, std::vector<ofPtr<mediaElement> > > bgVideos;

};
