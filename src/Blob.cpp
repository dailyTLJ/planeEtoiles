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
//    float destp[1][2] = {{0,0}};
    CvMat *pt_src = cvCreateMat(1,1, CV_32FC2);
    CvMat *pt_dst = cvCreateMat(1,1, CV_32FC2);
    float src[2] = {v.x*1, v.y*1};
    float dst[2] = {0,0};
    cvSetData(pt_src, src, 2*sizeof(float));
    cvSetData(pt_dst, dst, 2*sizeof(float));

    cvPerspectiveTransform(pt_src, pt_dst, perspectiveMat);

    CvScalar s;
    s = cvGet2D(pt_dst,0,0);
    cout << "transform " << v << " into " << s.val[0] << " | " << s.val[1] << endl;

    ofPoint newV;
//    newV.set( pt_dst->at<CV_32FC2>(0,0)[0], pt_dst->at<CV_32FC2>(0,0)[1] );
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
