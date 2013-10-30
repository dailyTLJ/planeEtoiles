#include "Blob.h"

Blob::Blob() {
	this->init();
//	cout << "Blob() init " << this->id << endl;
    // ofNotifyEvent(onCreate,this->id,this);
}

Blob::~Blob() {
}

//--------------------------------------------------------------
void Blob::init(){
    this->maxLifetime = 1;
	this->lifetime = this->maxLifetime;
	this->updated = true;

    this->vel = 0;
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
void Blob::follow(float x, float y, float frameW, float frameH, float margin){
    this->_rawPos.set(x, y);

    // on Edge?
    if (x<margin || x>frameW-margin || y<margin || y>frameH-margin) {
        this->onEdge = true;
    } else this->onEdge = false;

    this->newPosition = transformPerspective(this->_rawPos);

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
    this->velHistory.push_back(this->vel);
    while( this->velHistory.size() > VELOCITY_HISTORY ) {
        this->velHistory.erase( this->velHistory.begin() );
    }
}



//--------------------------------------------------------------
void Blob::analyze(float freezeMaxVel, float freezeMinTime, float freezeMaxTime, float movingThr) {
    if(this->vel < freezeMaxVel) {
        if(!frozen) {
            frozen = true;
            frozenStart = ofGetUnixTime();
            frozenTimer = 0;
        } else {
            frozenTimer = ofGetUnixTime() - frozenStart;
            if (frozenTimer >= freezeMinTime && !properFreeze) {
                properFreeze = true;
                ofNotifyEvent(onFreeze,this->id,this);
            }
            if (frozenTimer >= freezeMaxTime && !overFrozen) {
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
    if(this->velHistory.size() >= VELOCITY_HISTORY ) {
        float sum = 0;
        for (std::vector<float>::iterator it = this->velHistory.begin(); it != this->velHistory.end(); ++it) {
            sum += *it;
        }
        if (sum / this->velHistory.size() >= movingThr) this->movingMean = true;
        else this->movingMean = false;
    }
}

//--------------------------------------------------------------
void Blob::analyzeNeighbors(std::map<int, ofPoint> neighborLocation, float distStdDevThr, int steadyReward){

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
            float distance = ofDist(this->position.x, this->position.y, p.x, p.y);
            n->distance.push_back(distance);
            while( n->distance.size() > NEIGHBOR_HISTORY ) {
                n->distance.erase( n->distance.begin() );
            }
            n->updated = true;
            Pair pair = Pair( min(this->id,n->id), max(this->id,n->id) );
            if (n->distance.size() >= NEIGHBOR_HISTORY && n->getStdDev() < distStdDevThr) {
                if (!n->steadyDistance) {
                    n->steadyDistance = true;
                    n->steadyStart = ofGetUnixTime();
                    n->steadyTimer = 0;
                    ofNotifyEvent(onSteady,pair,this);
                } else {
                    n->steadyTimer = ofGetUnixTime() - n->steadyStart;
                    if (n->steadyTimer >= steadyReward && !n->steadyRewarded) {
                        n->steadyRewarded = true;
                        ofNotifyEvent(onSteadyReward,pair,this);
                        steadyRewarded = true;
                    }
                }
            } else {
                if (n->steadyDistance) {
                    n->steadyDistance = false;
                    n->steadyTimer = 0;
                    n->steadyRewarded = false;
                    ofNotifyEvent(onBreakSteady, pair, this);
                    steadyRewarded = false;     // this needs to come after the notifyEvent!
                }
            }


        }
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
void Blob::update(int minLostTime){
	if(this->updated == false) {
		this->lifetime--;
		return;
	} else {
        position = newPosition;
        if (videoTrace) ofNotifyEvent(updatePosition, this->id, this);
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
bool Blob::isAlive(){
	if(this->lifetime > 0) {
		return true;
	} else {
		return false;
	}
}
