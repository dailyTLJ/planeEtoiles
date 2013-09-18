#include "Blob.h"


Blob::Blob() {
	this->init();
}

//--------------------------------------------------------------
void Blob::init(){
    this->maxLifetime = 20;
	this->lifetime = this->maxLifetime;
	this->updated = true;
}

//--------------------------------------------------------------
void Blob::follow(int x, int y){
    this->_rawPos.set(x, y);
    this->position = transformPerspective(this->_rawPos);

    TimedPoint tPoint;
    tPoint.set(this->position.x, this->position.y);

    history.push_back(tPoint);

    while( history.size() > MAX_HISTORY ) {
        history.erase( history.begin() );
    }

	this->updated = true;
}

//--------------------------------------------------------------
ofPoint Blob::transformPerspective(ofPoint& v){
    // TODO: cv-perspectiveTransform
    ofPoint newV;
    newV.set( v.x, v.y );
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
