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
ofPoint Blob::transformPerspective(ofPoint& v){

    CvMat *pt_src = cvCreateMat(1,1, CV_32FC2);
    CvMat *pt_dst = cvCreateMat(1,1, CV_32FC2);
    cvSet1D(pt_src, 0, cvScalar(v.x, v.y));

    cvPerspectiveTransform(pt_src, pt_dst, perspectiveMat);
    CvScalar s = cvGet1D(pt_dst, 0);

//    cout << this->id << " transform " << v.x << " | " << v.y << " into " << cvRound(s.val[0]) << " | " << cvRound(s.val[1]) << endl;

    ofPoint newV;
    newV.set( s.val[0], s.val[1] );
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
