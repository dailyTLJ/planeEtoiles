#include "Blob.h"

Blob::Blob() {
	this->init();
}

Blob::~Blob() {
}

//--------------------------------------------------------------
void Blob::init(){
    this->maxLifetime = 2;
	this->lifetime = this->maxLifetime;
	this->updated = true;

    this->lost = false;
    this->occluded = false;
    this->vel = 0;
    this->onStage = false;
	this->frozen = false;
	this->properFreeze = false;
	this->overFrozen = false;
	this->frozenStart = 0;
	this->frozenTimer = 0;
	this->movingMean = false;
	this->onEdge = false;
    this->videoTrace = false;
    this->steadyRewarded = false;
}


//--------------------------------------------------------------
// raw blob position is updated from within oscReceive()
// can happen more than once, before follow(), depending on blobserver FPS
void Blob::setRawPosition(float x, float y) {
    this->_rawPos.set(x, y);
    this->updated = true;
}

//--------------------------------------------------------------
// actual processing of new raw-position (if updated)
// averages Y position, does perspectiveTransformation, and stores data in position history
void Blob::processRawPosition(float frameW, float frameH, float stageRadius, int y_mean){
    // ofLogNotice("BLOB") << "\t" << ofGetFrameNum() << "\t" << this->id << " processRawPosition()";
    // this->_rawPos.set(x, y);

    if (updated) {

        // average Y position
        int y_mean_min = min(y_mean, int(rawHistory.size()));
        float sum = _rawPos.y;
        for (int i=0; i<y_mean_min-1; i++) {
            sum += rawHistory[rawHistory.size()-1-i].point.y;
        }
        float avgY = sum / (float) y_mean_min;
        ofPoint avgP;
        avgP.set(_rawPos.x, avgY);

        this->position = transformPerspective(avgP);
        // this->position = transformPerspective(this->_rawPos);

        // on Stage?
        bool tmp = false;
        if ( abs(ofDist(this->position.x, this->position.y,frameW/2,frameH/2)) < stageRadius) tmp = true;
        if (onStage==true && tmp==false) ofNotifyEvent(onLeaveStage, this->id, this);
        else if (onStage==false && tmp==true) ofNotifyEvent(onEnterStage, this->id, this);
        onStage = tmp;

        TimedPoint rawPoint;
        rawPoint.set(_rawPos.x,_rawPos.y);
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
    }

	// this->updated = true;
}

//--------------------------------------------------------------
void Blob::setVelocity(float dx, float dy){
    this->velocity.set(dx, dy);
    this->vel = sqrt( pow(dx,2) + pow(dy,2) );
    this->velHistory.push_back(this->vel);
    while( this->velHistory.size() > VELOCITY_HISTORY ) {
        this->velHistory.erase( this->velHistory.begin() );
    }
}



//--------------------------------------------------------------
// see if blob is frozen, compute mean velocity, based on position history
void Blob::analyze(float freezeMaxVel, float freezeMinTime, float freezeMaxTime, float movingThr) {
    // only analyse freeze state, once the blob is old enough
    if(this->velHistory.size() >= VELOCITY_HISTORY ) {

        if(this->vel < freezeMaxVel) {
            if(!frozen) {
                frozen = true;
                frozenStart = ofGetUnixTime();
                frozenTimer = 0;
            } else {
                frozenTimer = ofGetUnixTime() - frozenStart;
                if (frozenTimer >= freezeMinTime && !properFreeze && !lost) {
                    properFreeze = true;
                    ofNotifyEvent(onFreeze,this->id,this);
                } else if (frozenTimer >= freezeMaxTime && !overFrozen && !lost) {
                    overFrozen = true;
                    ofNotifyEvent(overFreeze, this->id, this);
                }
            }
        } else {
            if(frozen) {
                frozen = false;
                frozenTimer = 0;
            }
            if(properFreeze) {
                properFreeze = false;
                ofNotifyEvent(unFreeze,this->id,this);
            }
            if(overFrozen) {
                overFrozen = false;
            }
        }

        // compute mean of velocity history
        float sum = 0;
        for (std::vector<float>::iterator it = this->velHistory.begin(); it != this->velHistory.end(); ++it) {
            sum += *it;
        }
        if (sum / this->velHistory.size() >= movingThr) this->movingMean = true;
        else this->movingMean = false;
    }
}

//--------------------------------------------------------------
// see if a blob is at a steady distance with one of its neighbor blobs
void Blob::analyzeNeighbors(std::map<int, tinyNeighbor> neighborLocation, float distStdDevThr, float minSteadyDistance){

    // set all neighbor-updates to false, to be able to delete inactive ones after
    for(std::map<int, Neighbor>::iterator it = neighbors.begin(); it != neighbors.end(); ++it){
        Neighbor* n = &it->second;
        n->updated = false;
    }

    vector<Pair> breakMe;   // store the neighbor-connections that are broken, to trigger them later

    // update neighbors with new location data
    for(std::map<int, tinyNeighbor>::iterator it = neighborLocation.begin(); it != neighborLocation.end(); ++it){
        int nid = it->first;
        if(nid != this-> id) {
            tinyNeighbor tn = it->second;
            ofPoint p = tn.position;

            // check if neighbor already exists in history
            std::map<int, Neighbor>::iterator iter = neighbors.find(nid);
            if( iter == neighbors.end() ) {
                Neighbor newN;
                neighbors[nid] = newN;
                neighbors[nid].id = nid;
            }

            // update neighbor with new location data
            Neighbor* n = &neighbors.find(nid)->second;
            float distance = ofDist(this->position.x, this->position.y, p.x, p.y);
            n->distance.push_back(distance);
            while( n->distance.size() > NEIGHBOR_HISTORY ) {
                n->distance.erase( n->distance.begin() );
            }
            n->updated = true;
            Pair pair = Pair( min(this->id,n->id), max(this->id,n->id) );
            if (n->distance.size() >= NEIGHBOR_HISTORY && n->getStdDev() < distStdDevThr && movingMean && onStage && tn.movingMean && tn.onStage && distance > minSteadyDistance) {
                if (!n->steadyDistance) {
                    n->steadyDistance = true;
                    n->steadyStart = ofGetUnixTime();
                    n->steadyTimer = 0;
                    ofNotifyEvent(onSteady,pair,this);
                } else {
                    n->steadyTimer = ofGetUnixTime() - n->steadyStart;
                }
            } else {
                if (n->steadyDistance) {
                    n->steadyDistance = false;
                    n->steadyTimer = 0;
                    n->steadyRewarded = false;
                    breakMe.push_back(pair);
                }
            }


        }
    }

    // set the blobs global steadyRewarded var, based on steadyRewarded-info from all neighbors
    bool stillSteady = false;
    for(std::map<int, Neighbor>::iterator it = neighbors.begin(); it != neighbors.end(); ++it){
        Neighbor* n = &it->second;
        if (n->steadyRewarded) stillSteady = true;
    }
    if (!stillSteady && steadyRewarded) {
        steadyRewarded = false;     // this needs to come after the notifyEvent!
    }

    // now trigger breaksteady, because its knows if to switch the star video as well
    for(vector<Pair>::iterator it = breakMe.begin(); it != breakMe.end(); ++it){
        ofNotifyEvent(onBreakSteady, (*it), this);
    }

    // delete not updated neighbors, because they are gone!
    std::map<int,Neighbor>::iterator it = neighbors.begin();
    while (it != neighbors.end()) {
    	if( !it->second.updated ) {
            if (it->second.steadyDistance) {
                Pair pair = Pair( min(this->id,it->second.id), max(this->id,it->second.id) );
                ofNotifyEvent(onBreakSteady,pair,this);
            }
    		neighbors.erase(it++);
    	} else {
    		++it;
    	}
    }

}

//--------------------------------------------------------------
// transform the rawPosition with a perspective Matrix,
// to stretch and rotate the camera-view into a top-down view
ofPoint Blob::transformPerspective(ofPoint& v){

    vector<cv::Point2f> pre, post;
    pre.push_back(cv::Point2f(v.x, v.y));

    cv::perspectiveTransform(pre, post, *perspectiveMat);

    ofPoint newV;
    newV.set( post[0].x, post[0].y );
    return newV;
}

//--------------------------------------------------------------
// check blob lifetime, and trigger inLost event
void Blob::update(int minLostTime){
    // ofLogNotice("BLOB") << "\t" << ofGetFrameNum() << "\t" << this->id << " update()";
	if(this->updated == false) {
		this->lifetime--;
		return;
    }
    this->lifetime = this->maxLifetime;
	this->updated = false;
	// now do analysis
    if(this->lostDuration > minLostTime) {
        if (!lost) {
            lost = true;
            ofNotifyEvent(onLost,this->id,this);
        }
    } else {
        lost = false;
    }
}

//--------------------------------------------------------------
// if the blob is connected to a video > 
// trigger updatePosition event to update the video position
void Blob::updateVideo() {
    if (isAlive() && videoTrace) ofNotifyEvent(updatePosition, this->id, this);
}

//--------------------------------------------------------------
bool Blob::isAlive(){
	if(this->lifetime > 0) {
        if (position.x == 0 && position.y == 0) {
            ofLogNotice("BLOB") << "\t" << ofGetFrameNum() << "\t" << this->id << " isAlive() - position 0|0";
            return false;
        }
		return true;
	} else {
		return false;
	}
}
