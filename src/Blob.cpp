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

//--------------------------------------------------------------
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
void Blob::analyzeNeighbors(std::map<int, ofPoint> neighborLocation){

    // set all neighbor-updates to false, to be able to delete inactive ones after
    for(std::map<int, Neighbor>::iterator it = neighbors.begin(); it != neighbors.end(); ++it){
        Neighbor* n = &it->second;
        n->updated = false;
    }

    // update with new location data
    for(std::map<int, ofPoint>::iterator it = neighborLocation.begin(); it != neighborLocation.end(); ++it){
        int nid = it->first;
        if(nid != this-> id) {
            ofPoint p = it->second;

            // check if neighbor already exists in history
            std::map<int, Neighbor>::iterator iter = neighbors.find(nid);
            if( iter == neighbors.end() ) {
                Neighbor newN;
                neighbors[nid] = newN;
                neighbors[nid].id = nid;
            }

            // update neighbor with new location data
            Neighbor* n = &neighbors.find(nid)->second;
            float distance = ofVec2f(this->position.x, this->position.y).distance( ofVec2f(p.x, p.y) );
            n->distance.push_back(distance);
            while( n->distance.size() > NEIGHBOR_HISTORY ) {
                n->distance.erase( n->distance.begin() );
            }
            n->updated = true;
        }
    }


    // delete not updated neighbors, because they are gone!
    std::map<int,Neighbor>::iterator it = neighbors.begin();
    while (it != neighbors.end()) {
    	if( !it->second.updated ) {
    		neighbors.erase(it++);
    	} else {
    		++it;
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
