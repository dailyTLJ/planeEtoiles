#include "Blob.h"


Blob::Blob() {
	this->init();
}

//--------------------------------------------------------------
void Blob::init(){
	this->lifetime = this->maxLifetime;
	this->updated = true;
}

//--------------------------------------------------------------
void Blob::follow(){
	this->updated = true;
}

//--------------------------------------------------------------
void Blob::transformPerspective(){

}

//--------------------------------------------------------------
void Blob::update(){
	if(this->updated == false) {
		this->lifetime--;
		return;
	}

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