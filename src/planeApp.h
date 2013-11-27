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
        int length[7];
        string instructions[2][7];
        string analysis[7];
};

class planeApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
		void exit();

		void receiveOsc();

		void initScenes();
		void setPerspective();
		void recalculatePerspective(int & v);

		void drawRawData(int x, int y, float scale);
		void drawTopDown(int x, int y, float scale, bool detailed = false);
		void drawScreen(int x, int y, float scale);
		void drawAnalysis(int x, int y, float scale);
		void drawControlInfo(int x, int y);

		// to be triggered via blob/media ofEvents
		void blobOnCreate(int & blobID);
		void blobOnLost(int & blobID);
		void blobOnFreeze(int & blobID);
		void blobUnFreeze(int & blobID);
		void blobOverFreeze(int & blobID);
		void blobUnlink(int & blobID);
		void blobSteady(Pair & pair);
		void blobSteadyReward(Pair & pair);
		void blobBreakSteady(Pair & pair);
		void videoFollowBlob(int & blobID);
		void blobEnterStage(int & blobID);
		void blobLeaveStage(int & blobID);

		void blobCountChange();
		void jumpToScene(int s);
		bool allBlobsAlignedWith(ofPoint &p);

		void endSegment(int direction = 1);	// 1. trigger fgMediaFadedOut 
		void fgMediaFadedOut(int & trans);  // 2. call outroTransformation calls on FG and BG media
		void bgMediaFadedOut(int & trans);	// 3. all elements faded out, moveOn = true
		void nextSegment(int direction = 1);// 4. pick the next segment
		void initSegment();					// 5. initialize the new segment, create new fgvideos
		void beginSegment();				// 6. after flash, fade in BG
		void bgMediaFadedIn(int & trans);	// 7. reinit blobs, introtransformation of videos
		void fgMediaFadedIn(int & trans);


		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		int language;

		int projectionW;
        int projectionH;
        int blobserverW;
        int blobserverH;
        int blobW;
        int blobH;
        float cameraExposure;

		int mouseX, mouseY;
		string mouseButtonState;

		std::map<int, Blob> blobs;
		// Blob testBlob;
		cv::Mat perspectiveMat;
		ofPoint steles[8];
		ofPoint steles_topdown[8];

		bool fullscreen;	

		// SEQUENCING	 
		bool processing;
		bool oscMsgReceived;
		bool oscActive;
		int oscLastMsg;
		int oscLastMsgTimer;
		int oscLastMsgTimerMax;
        ofxToggle autoplay; 			 // advance to next segment by itself
		int scene;
		int segment;
		int segmentChange;
		bool sceneChange;
		int masterClock;
		int segmentClock;
		int segmentStart;
		int globalStart;
		bool moveOn;
		bool transition;
		bool success;			// if people followed the instructions
		int successCnt;
		bool flash;				// in between transitions, catch attention
		int flashCnt;
		int flashMax;

		bool drawBridge;
		float bridgeX;
		float bridgeY;

		std::map<int, sceneInfo> scenes;

		bool drawBlobDetail;

		int bgsubtractorCnt;
		int bgsubtractorFlowId;
		float bgsubtractorVel;	
		float bgsubtractorAvVel;

		ofTrueTypeFont fontBg;
		ofTrueTypeFont fontSm;
		ofxPanel gui;
		ofParameterGroup paramBasic;
		ofParameterGroup paramTiming;
		ofParameterGroup paramSc1;
		ofParameterGroup paramSc2;
		ofParameterGroup paramSc3;
		ofParameterGroup paramSc4;
		ofParameterGroup paramSc5;
        ofParameter<ofColor> flashColor;
		ofxOscReceiver receiver;

		ofParameter<float> freezeMaxVel;
		ofParameter<int> freezeMinTime;
		ofParameter<int> freezeMaxTime;

		ofParameter<float> distStdDevThr;
		ofParameter<float> movingThr;
		ofParameter<int> steadyRewardTime;
		ofParameter<int> edgeMargin;

		ofParameter<int> siteW;
		ofParameter<int> siteH;
		ofParameter<float> mapSiteW;
		ofParameter<float> mapSiteH;
		ofParameter<int> offsetX;
		ofParameter<int> offsetY;
		ofParameter<int> stageRadius;

		ofParameter<int> newStarMax;
		ofParameter<int> newStarBonus;
		ofParameter<int> minLostTime;

		ofParameter<int> alignmentMaxDist;
		ofParameter<int> alignmentTransition;

		std::vector< ofPtr<mediaElement> > fgMedia;
//		std::map<int, std::vector<videoElement> > fgVideos;
		std::map<int, std::vector<ofPtr<mediaElement> > > bgVideos;

};
