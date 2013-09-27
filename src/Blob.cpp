#include "Blob.h"


Blob::Blob() {
	this->init();
}

//--------------------------------------------------------------
void Blob::init(){
    this->maxLifetime = 20;
	this->lifetime = this->maxLifetime;
	this->updated = true;

    this->vel = 0;
	this->frozen = false;
	this->frozenStart = 0;
	this->frozenTimer = 0;
}

//--------------------------------------------------------------
void Blob::follow(float x, float y){
    this->_rawPos.set(x, y);
    this->position = transformPerspective(this->_rawPos);

    TimedPoint rawPoint;
    rawPoint.set(x,y);
    rawHistory.push_back(rawPoint);

    TimedPoint tPoint;
    tPoint.set(this->position.x, this->position.y);

    history.push_back(tPoint);

    while( rawHistory.size() > MAX_HISTORY ) {
        rawHistory.erase( rawHistory.begin() );
    }

    while( history.size() > MAX_HISTORY ) {
        history.erase( history.begin() );
    }

	this->updated = true;
}

//--------------------------------------------------------------
void Blob::setVelocity(float dx, float dy){
    this->velocity.set(dx, dy);
    this->vel = sqrt( pow(dx,2) + pow(dy,2) );
}

void Blob::analyze(float freezeMinVel){
    if(this->vel < freezeMinVel) {
        if(!frozen) {
            frozen = true;
            frozenStart = ofGetUnixTime();
            frozenTimer = 0;
        } else {
            frozenTimer = ofGetUnixTime() - frozenStart;
        }
    } else {
        if(frozen) {
            frozen = false;
            frozenTimer = 0;
        }
    }
}

//--------------------------------------------------------------
ofPoint Blob::transformPerspective(ofPoint& v){

    float pv[2] = {v.x, v.y};

    vector<cv::Point2f> pre, post;
    pre.push_back(cv::Point2f(v.x, v.y));

    cv::perspectiveTransform(pre, post, *perspectiveMat);

    ofPoint newV;
    newV.set( post[0].x, post[0].y );
    return newV;
}

//--------------------------------------------------------------
void Blob::update(){
	if(this->updated == false) {
		this->lifetime--;
		return;
	}
    this->lifetime = this->maxLifetime;
	this->updated = false;
	// now do analysis
}

//--------------------------------------------------------------
bool Blob::isAlive(){
	if(this->lifetime > 0) {
		return true;
	} else {
		return false;
	}
}
