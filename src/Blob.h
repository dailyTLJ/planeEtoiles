#pragma once

#define MAX_HISTORY 100

class TimedPoint {
	ofVec2f point;
	int time;

	void set(int x, int y) {
		point.set( x, y );
		// set time to current time
	}
};

class Blob {

	int id;

	// information coming from blobserver
	ofVec2f _rawPos;
	ofVec2f position;
	ofVec2f velocity;
	float size;
	float age;
	float lostDuration;

	TimedPoint history[MAX_HISTORY];

	// internal information
	int lifetime;
	int maxLifetime = 10;
	bool updated;	// updated by last sample ? 

	// trail analysis


	Blob() {}
	void init();
	void follow();
	void transformPerspective();
	void update();
	bool isAlive();

};