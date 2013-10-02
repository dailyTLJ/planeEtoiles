#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "ofxOpenCv.h"
#include "ofxGui.h"

#include <map>
#include "Blob.h"
#include "videoElement.h"

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

		int projectionW;
        int projectionH;

		void drawRawData(int x, int y, float scale);
		void drawTopDown(int x, int y, float scale, bool detailed = false);
		void drawScreen(int x, int y, float scale);
		void drawAnalysis(int x, int y, float scale);
		void drawControlInfo(int x, int y);
		void nextSegment(int direction);

		ofTrueTypeFont font;
		ofxPanel gui;
		ofxOscReceiver receiver;

		int mouseX, mouseY;
		string mouseButtonState;

		std::map<int, Blob> blobs;
		Blob testBlob;
		cv::Mat perspectiveMat;

        bool autoplay;  // advance to next segment by itself
		int scene;
		int segment;
		int masterClock;
		int segmentClock;
		int segmentStart;
		int globalStart;

		std::map<int, sceneInfo> scenes;

		bool drawBlobDetail;

		ofParameter<float> freezeMinVel;
		ofParameter<int> freezeMinTime;
		ofParameter<int> freezeMaxTime;

		ofParameter<float> keepDistanceThr;
		ofParameter<float> movingThr;

		std::map<int, std::vector<videoElement> > bgVideos;
		std::map<int, std::vector<videoElement> > fgVideos;

};
