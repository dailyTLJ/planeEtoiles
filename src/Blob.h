#pragma once

#define MAX_HISTORY 100

class TimedPoint {
	vofVec2f point;
	int time;

	void set(int x, int y) {
		point.set( x, y );
		// set time to current time
	}
};

class Blob {

	int id;

	vofVec2f _rawPos;
	vofVec2f position;
	vofVec2f velocity;
	float size;
	float age;
	float lostDuration

	TimedPoint history[MAX_HISTORY];

	Blob() {}
	void init();
	void follow();
	void transformPerspective();
	void update();
	bool isAlive();

};