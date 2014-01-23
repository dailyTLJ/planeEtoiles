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
#include "textElement.h"

// listen to blobserver on port 9000
#define MYPORT 9000
#define BLOBPORT 9002
// #define BLOBSERVERIP "192.168.2.12"
// #define BLOBSERVERIP "192.168.2.14"
#define BLOBSERVERIP "10.4.40.28"
// #define MYIP "192.168.2.39"
#define MYIP "10.10.44.21"

// define scenes
#define IDLE 			0
#define STARS 			1
#define ATTRACTION	 	2
#define REVOLUTIONS 	3
#define SUN 			4
#define ECLIPSE 		5
#define SHOOTING 		6
// define special segment
#define SEG_FOLLOWME 	1
#define MAX_SEGMENTS	8

class sceneInfo {
    public:
        string name;
        int no;
        int segments;
        int length[MAX_SEGMENTS];
        string instructions[3][MAX_SEGMENTS];
        string instructionImg[2][MAX_SEGMENTS];
        string instructionVid[2][MAX_SEGMENTS][3];
        string analysis[MAX_SEGMENTS];
};

class planeApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
		void exit();

		void receiveOsc();
		void sendOscMsg(string addr, string v1, int v2);
		void sendOscMsgToHog(string addr, string v1, float v2);


		void initScenes();
		void setPerspective();
		void recalculatePerspective(int & v);

		void drawRawData(int x, int y, float scale, bool displayText = false);
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
		void bridgeUnlink(int & blobID);
		void bridgeUnlink(Pair & pair);
		void blobSteady(Pair & pair);
		void blobSteadyReward(Pair & pair);
		void blobBreakSteady(Pair & pair);
		void videoFollowBlob(int & blobID);
		void blobEnterStage(int & blobID);
		void blobLeaveStage(int & blobID);


		void positionRevolutions();
		void jumpToScene(int s);
		bool allBlobsAlignedWith(ofPoint &p);
		ofPoint blobMapToScreen(ofPoint &o);
			// 
		void endSegment();					// 1. 
		void endedInstructions(int & trans);// trigger fgMediaFadedOut or moveOn
		void fgMediaFadedOut(int & trans);  // 2. call outroTransformation calls on FG and BG media
		void allFaded(int & trans); 		// 3. all elements faded out, moveOn = true
		void bgMediaSwap(int & trans);		//
		void nextSegment();					// 4. pick the next segment
		void initSegment();					// 5. initialize the new segment, create new fgvideos
		void configureBlobserver();
		void beginSegment();				// 6. after flash, fade in BG
		// void bgMediaFadedIn(int & trans);	// 7. reinit blobs, introtransformation of videos
		// void fgMediaFadedIn(int & trans);
		void unHideSun(int & trans);
		void placeInstruction(int & trans);

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		void guiNebulaChange(int & v);

		void printDebugInfo();

		std::stringstream coutput;

		int language;
		int languageCnt;
		ofParameter<int> languageRatio;
		
		bool projectorOn;
		int projectionW;
        int projectionH;
        int projectionOffsetX;
        int blobserverW;
        int blobserverH;
        int blobW;
        int blobH;
        float cameraExposure;
        int moonPosX;

		int mouseX, mouseY;
		string mouseButtonState;

		std::map<int, Blob> blobs;
		// Blob testBlob;
		cv::Mat perspectiveMat;
		ofPoint steles[8];
		ofPoint steles_topdown[8];

		// SEQUENCING
		bool processing;
		bool oscMsgReceived;
		bool oscActive;
		bool oscNetworkReady;
		float oscLastMsg;
		float oscLastMsgTimer;
		float oscLastMsgTimerMax;
        ofxToggle autoplay; 			 // advance to next segment by itself
        ofxToggle testMode;
		int scene;
		int segment;
		// int segmentChange;
		bool sceneChange;
		int masterClock;
		int segmentClock;
		int lastActivityClock;
		int lastActivity;
		bool resetClock;
		int segmentStart;
		int globalStart;
		bool moveOn;
		bool transition;
		bool success;			// if people followed the instructions
		int successCnt;
		int activityCnt;
		int blobsOnStage;
		int planetCnt;
		int pickPlanet;
		bool flash;				// in between transitions, catch attention
		int flashCnt;
		int flashMax;

		int shootingPointer;

		bool drawDebugScreen;
		bool drawBridge;
		float bridgeX;
		float bridgeY;

		std::map<int, sceneInfo> scenes;

		bool drawBlobDetail;

		int currentFlowId;
		int bgsubtractorCnt;
		int bgsubtractorFlowId;
		int hogFlowId;
		string hogFlowName;
		float bgsubtractorVel;
		float bgsubtractorAvVel;
		float hogAvVel;

		ofTrueTypeFont fontBg;
		ofTrueTypeFont fontIdle;
		ofTrueTypeFont fontSm;
		ofxPanel gui;
		ofxPanel languageGui;
		ofParameterGroup paramBasic;
		ofParameterGroup paramTiming;
		ofParameterGroup paramSc1;
		ofParameterGroup paramSc2;
		ofParameterGroup paramSc3;
		ofParameterGroup paramSc4;
		ofParameterGroup paramSc5;
		ofParameterGroup paramSc6;
		ofParameterGroup paramBlob;
        ofParameter<ofColor> flashColor;
        ofParameter<int> nebulaOpacity;
		ofxOscReceiver receiver;
		ofxOscSender sender;

		ofParameter<float> instructionFadeIn;
		ofParameter<float> instructionFadeOut;

		ofParameter<float> freezeMaxVel;
		ofParameter<float> freezeAllMaxVel;
		ofParameter<int> freezeMinTime;

		ofParameter<int> freezeMaxTime;
		ofParameter<int> minSegmentLength;
		ofParameter<int> inactivityTimer;

		ofParameter<float> distStdDevThr;
		ofParameter<float> movingThr;
		ofParameter<int> steadyRewardTime;
		ofParameter<int> edgeMargin;
		ofParameter<int> activityColorCh;

		ofParameter<int> siteW;
		ofParameter<int> siteH;
		ofParameter<float> mapSiteW;
		ofParameter<float> mapSiteH;
		ofParameter<int> offsetX;
		ofParameter<int> offsetY;
		ofParameter<int> stageRadius;
		ofParameter<int> y_mean;

		ofParameter<int> newStarMax;
		// ofParameter<int> newStarBonus;
		ofParameter<int> minLostTime;
		ofParameter<int> minLostHop;
		ofParameter<int> minLostSpin;
		ofParameter<int> minLostShoot;

		ofParameter<int> spinSuccess;
		ofParameter<int> spinFailure;
		ofParameter<int> spinJudgeTime;
		ofParameter<int> freezeJudgeTime;
		ofParameter<float> freezeVideoSpeedMap;
		ofParameter<int> runJudgeTime;
		ofParameter<float> runHogThr;
		ofParameter<float> runBgsThr;
		ofParameter<int> runActThr;

		ofParameter<int> alignmentMaxDist;
		ofParameter<int> alignmentTransition;
		ofParameter<float> followMeSpeed;
		ofParameter<int> followMeRadius;
		float followMe;

		ofxToggle configBlobserver;
		ofxToggle displayDebug;
		ofParameter<int> noiseSlow;
		ofParameter<int> noiseNormal;
		ofParameter<int> noiseErratic;

		ofParameter<int> measurementSlow;
		ofParameter<int> measurementNormal;
		ofParameter<int> measurementErratic;


		// vector holding foreground movies
		// that are loaded dynamically
		std::vector< ofPtr<mediaElement> > fgMedia;

		// vector holds bg movies:
		// 4 different idle mode animations
		// starry-bg intro, starry-bg loop, sun-intro)
		std::vector< ofPtr<mediaElement> > bgMedia;

		// nebula movie, that is placed underneath
		// the bgMedia movies
		ofPtr<mediaElement> nebula;


		// videofiles that should be preloaded, as they are larger:
		// STARS
		// ATTRACTION
		ofPtr<mediaElement> attraction_outro;
		// REVOLUTIONS
		// SUN
		ofPtr<mediaElement> sun_jump;
		ofPtr<mediaElement> sun_run;
		std::vector< ofPtr<mediaElement> > sun_freeze_red;			// 4
		std::vector< ofPtr<mediaElement> > sun_surface_blue;		// 12
		// ECLIPSE
		std::vector< ofPtr<mediaElement> > planet_animated;			// 9
		// SHOOTING
		std::vector< ofPtr<mediaElement> > shooting_stars;			// 50
		std::vector< ofPtr<mediaElement> > title_sequence;			// 2
		std::vector< ofPtr<mediaElement> > diagram_sequence;		// 2

		ofImage instructionImg;
		ofPtr<mediaElement> instructionVid;
		textElement instructionTxt;
		int bgMediaId;

};
