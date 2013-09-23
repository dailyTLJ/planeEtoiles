#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "ofxOpenCv.h"

#include <map>
#include "Blob.h"

// listen to blobserver on port 9000
#define PORT 9000
#define NUM_MSG_STRINGS 20

class planeApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
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

		ofTrueTypeFont font;
		ofxOscReceiver receiver;

		int mouseX, mouseY;
		string mouseButtonState;

		std::map<int, Blob> blobs;
		Blob testBlob;
		cv::Mat perspectiveMat;

		int scene;
		int segment;
		int masterClock;

};
