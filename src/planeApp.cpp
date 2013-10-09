#include "planeApp.h"
#include "ofBitmapFont.h"

//--------------------------------------------------------------
void planeApp::setup(){

	ofTrueTypeFont::setGlobalDpi(72);

    font.loadFont("Mercury Bold.otf", 36, true, true);
	font.setLineHeight(30.0f);
	font.setLetterSpacing(1.037);

	mouseX = 0;
	mouseY = 0;
	mouseButtonState = "";

    autoplay = false;
	drawBlobDetail = true;

//    siteW = 500;
//    siteH = 500;
    projectionW = 1080;
    projectionH = 1920;
    blobserverW = 320;
    blobserverH = 240;
    blobW = 64;
    blobH = 128;

    siteW.addListener(this,&planeApp::recalculatePerspective);
	siteH.addListener(this,&planeApp::recalculatePerspective);

	gui.setup("panel", "settings.xml", 1164,250);
	gui.setSize(250,50);
	gui.setDefaultBackgroundColor( ofColor(0,0,50) );
	gui.add(siteW.set( "Site Width", 500, 0, 1000));
    gui.add(siteH.set( "Site Length", 700, 0, 1000));
    gui.add(mapSiteW.set( "Map Site to Viz Width", 2, 0, 4));
    gui.add(mapSiteH.set( "Map Site to Viz Height", 2, 0, 4));
    gui.add(offsetX.set( "Offset X", 0, -500, 500));
	gui.add(offsetY.set( "Offset Y", 0, -500, 500));

	gui.add(freezeMinVel.set( "Freeze Minimum Velocity",0.1, 0, 1.0 ));
	gui.add(freezeMinTime.set( "Freeze Minimum Time",2, 0, 5 ));
	gui.add(freezeMaxTime.set( "Freeze Maximum Time",10, 0, 30 ));
	gui.add(keepDistanceThr.set( "Distance Std.Dev. Threshold", 10, 0, 20));
	gui.add(movingThr.set( "Movement Threshold", 0.1, 0, 2));
	gui.add(edgeMargin.set( "Edge Margin", 50, 0, 150));
	gui.add(hopLength.set( "Hop Length", 5, 0, 35));

    // init
	this->initScenes();

    // compute the perspectiveTransformation
    // to map from blob-coordinates to the top-down view
    this->setPerspective();

    // test blob to verify perspective Mat
//    testBlob.perspectiveMat = &perspectiveMat;
//    for( int i=0; i<50; i++ ) {
//        testBlob.follow( i*3, i * 3.0/5.0 );
//        testBlob.follow(150-i*3,i * 3.0/5.0);
//    }

    // listen on the given port
	cout << "listening for osc messages on port " << PORT << "\n";
	receiver.setup(PORT);

	ofBackground(255);

}

void planeApp::recalculatePerspective(int & v) {
    cout << "recalculatePerspective" << endl;
    cout << "value " << v << endl;
    cout << "siteW " << siteW << "   siteH " << siteH << endl;
    this->setPerspective();
}

void planeApp::setPerspective() {
    int skew = 0;
    // 800x600  scale 0.4 >> 320x240   mninus 64x128
    float in[] = {skew,0,blobserverW-skew,0,blobserverW,blobserverH,0,blobserverH};
    float out[] = {0,0,siteW,0,siteW,siteH,0,siteH};

    cv::Mat proj_in(4,2, CV_32FC1, in);
    cv::Mat proj_out(4,2, CV_32FC1, out);
    perspectiveMat = cv::findHomography(proj_in, proj_out);
    proj_in.release();
    proj_out.release();

    // if there are already blobs?
}

//--------------------------------------------------------------
void planeApp::initScenes(){

    int n = 0;

    globalStart = ofGetUnixTime();
    segmentStart = ofGetUnixTime();
    scene = -1;
	segment = 0;
	segmentClock = 0;
	masterClock = 0;



    sceneInfo idle;
    idle.name = "Idle";
    idle.no = n;
    idle.segments = 1;
    idle.instructions[0] = "Come closer!";
    idle.length[0] = -1;
    scenes[n] = idle;
 
    bgVideos[n].push_back(videoElement("video/2_stars/BACKGROUND 1 loop-QTAnimation.mov"));
//    bgVideos[n][bgVideos[n].size()-1].setDisplay(200,0);

    n++;

    sceneInfo stars;
    stars.name = "Star Links";
    stars.no = n;
    stars.segments = 4;
    stars.instructions[0] = "Stand still.";
    stars.length[0] = 20;
    stars.instructions[1] = "Try new spots \nto light up \nmore stars.";
    stars.length[1] = 20;
    stars.instructions[2] = "Walk with \nsomeone. \nKeep the \nsame distance \nbetween you.";
    stars.length[2] = 30;
    stars.instructions[3] = "Walk with \nsomeone. \nMake eye \ncontact. \nKeep the \ndistance.";
    stars.length[3] = 30;
    scenes[n] = stars;

    bgVideos[n].push_back(videoElement("video/2_stars/BACKGROUND 1 loop-QTAnimation.mov"));

    n++;

    sceneInfo revolution;
    revolution.name = "Revolution";
    revolution.no = n;
    revolution.segments = 2;
    revolution.instructions[0] = "Take someone's \nhand. \nSpin and lean \nout as far as \npossible.";
    revolution.length[0] = 20;
    revolution.instructions[1] = "Let go.";
    revolution.length[1] = 20;
    scenes[n] = revolution;

    bgVideos[n].push_back(videoElement("video/2_stars/BACKGROUND 1 loop-QTAnimation.mov"));


    n++;

    sceneInfo sun;
    sun.name = "Explosion";
    sun.no = n;
    sun.segments = 6;
    sun.instructions[0] = "Stand on one leg.";
    sun.length[0] = 20;
    sun.instructions[1] = "Hope from one \nspot to the other.";
    sun.length[1] = 20;
    sun.instructions[2] = "Hope from one \nspot to the other.\nEveryone in unison.";
    sun.length[2] = 20;
    sun.instructions[3] = "FREEZE!";
    sun.length[3] = 20;
    sun.instructions[4] = "Run in every \ndirection at once.";
    sun.length[4] = 20;
    sun.instructions[5] = "FREEZE!";
    sun.length[5] = 20;
    scenes[n] = sun;

    bgVideos[n].push_back(videoElement("video/2_stars/BACKGROUND 1 loop-QTAnimation.mov"));

    n++;

    sceneInfo eclipse;
    eclipse.name = "Alignment";
    eclipse.no = n;
    eclipse.segments = 4;
    eclipse.instructions[0] = "Now align \nyourself in \nfront of me.\nAnd follow me.";
    eclipse.length[0] = 20;
    eclipse.instructions[1] = "Step out \nof the line.";
    eclipse.length[1] = 20;
    eclipse.instructions[2] = "Step into \nthe line.";
    eclipse.length[2] = 30;
    eclipse.instructions[3] = "Disperse very \nslowly towards \nthe edges.";
    eclipse.length[3] = 30;
    scenes[n] = eclipse;

    bgVideos[n].push_back(videoElement("video/2_stars/BACKGROUND 1 loop-QTAnimation.mov"));

    n++;

    sceneInfo shooting;
    shooting.name = "Combustion";
    shooting.no = n;
    shooting.segments = 4;
    shooting.instructions[0] = "Move like a \nshooting star.";
    shooting.length[0] = 20;
    shooting.instructions[1] = "Drop to the \nground!";
    shooting.length[1] = 20;
    shooting.instructions[2] = "Exhale.\nLook at the sky.";
    shooting.length[2] = 30;
    shooting.instructions[3] = "Stand up.";
    shooting.length[3] = 30;
    scenes[n] = shooting;

    bgVideos[n].push_back(videoElement("video/2_stars/BACKGROUND 1 loop-QTAnimation.mov"));

    cout << "there are " << scenes.size() << " scenes" << endl;

    nextSegment(1);

}

//--------------------------------------------------------------
void planeApp::update(){

    this->receiveOsc();

	// check for dead blobs, and delete them from list
    std::map<int,Blob>::iterator it = blobs.begin();
    while (it != blobs.end()) {
        Blob* b = &it->second;
    	b->update();
    	if( !b->isAlive() ) {
//            cout << "erase blob " << b->id << endl;
            ofNotifyEvent(b->prepareToDie,b->id,b);
    		blobs.erase(it++);
    	} else {
    		++it;
    	}
    }

    masterClock = ofGetUnixTime() - globalStart;
    segmentClock = ofGetUnixTime() - segmentStart;

    // move on to next segment?
    // simply based on time for now
    if(autoplay && segmentClock >= 5){
        nextSegment(1);
    }

    // analysis
    std::map<int, ofPoint> blobPositions;
    for(std::map<int, Blob>::iterator it = blobs.begin(); it != blobs.end(); ++it){
        Blob* b = &it->second;
        blobPositions[b->id] = b->position;
    }
    for(std::map<int, Blob>::iterator it = blobs.begin(); it != blobs.end(); ++it){
        Blob* b = &it->second;
        b->analyzeNeighbors(blobPositions, keepDistanceThr);
    }

    // videos. only update the ones currently on display ?
    for (vector<videoElement>::iterator it = bgVideos[scene].begin() ; it != bgVideos[scene].end(); ++it) {
        videoElement* v = &(*it);
        v->update();
    }

    for (vector<ofPtr<mediaElement> >::iterator it = fgMedia.begin(); it != fgMedia.end(); ++it) {
        (**it).update();
    }


    if (scene==1) {
        if (segment==0 || segment==1) {

            // STAND STILL

        }
    }

}


void planeApp::blobOnFreeze(int & blobID) {
//    cout << "BLOB " << blobID << " froze!" << endl;

    if (scene==1) {
        if (segment==0 || segment==1) {

            // STAND STILL
            // create new video element
//            cout << "blob " << blobID << " \t\tfreeze ! " << endl;
            fgMedia.push_back(ofPtr<mediaElement>( new videoElement("video/2_stars/INDIV STAR 2 loop-H264-10mbps.mp4")));
            blobs[blobID].mediaLink = fgMedia[fgMedia.size()-1];
            (*fgMedia[fgMedia.size()-1]).setDisplay(ofRandom(projectionW-100), ofRandom(projectionH-100));
            (*fgMedia[fgMedia.size()-1]).reset();
        }
    }
}

void planeApp::blobUnFreeze(int & blobID) {

    if (scene==1) {
        if (segment==0 || segment==1) {
//            cout << "BLOB " << blobID << " moved again!  mediaLink = " << blobs[blobID].mediaLink << endl;
            // STAND STILL
            this->blobUnlink(blobID);
//            for (vector<ofPtr<videoElement> >::iterator it = fgMedia.begin(); it != fgMedia.end(); it++) {
//                if (*it == blobs[blobID].mediaLink) {
//                    fgMedia.erase(it);
//                    break;
//                }
//            }
        }
    }
}

void planeApp::blobOverFreeze(int & blobID) {
    if (scene==1) {
        if (segment==1) {
            // no more video, been frozen too long!
            this->blobUnlink(blobID);
        }
    }
}

void planeApp::blobOnCreate(int & blobID) {
   


    // first clean up, all related blob settings
    this->blobUnlink(blobID);
    blobs[blobID].videoTrace = false;
    // blobs[blobID].mediaLink = NULL;  // TODO how to release ofPtr ? 

    // then assign the appropriate ones
    if (scene==1) {
        if (segment==2 || segment==3) {
            cout << "BLOB " << blobID << " blobOnCreate!" << endl;
            // KEEP THE DISTANCE
            // create new video element
//            cout << "blob " << blobID << " \t\tfreeze ! " << endl;
            fgMedia.push_back(ofPtr<mediaElement>( new videoElement("video/2_stars/INDIV STAR 2 loop-H264-10mbps.mp4")));
            blobs[blobID].mediaLink = fgMedia[fgMedia.size()-1];
            blobs[blobID].videoTrace = true;
            // (*fgMedia[fgMedia.size()-1]).setDisplay(ofRandom(projectionW-100), ofRandom(projectionH-100));
            (*fgMedia[fgMedia.size()-1]).reset();
        }
    } else if (scene==4) {
        // PLANETS
        int randomPlanet = ofRandom(24) + 1;
        fgMedia.push_back(ofPtr<mediaElement>( new imageElement("video/5_eclipse/PLANET_" + ofToString(randomPlanet)+".png")));
        blobs[blobID].mediaLink = fgMedia[fgMedia.size()-1];
        blobs[blobID].videoTrace = true;
    }
}

void planeApp::videoFollowBlob(int & blobID) {
    // cout << "blob " << blobID << " \t\tvideoFollowBlob" << endl;
    // find videoElement
    ofPtr<mediaElement> vid; 
    for (vector<ofPtr<mediaElement> >::iterator it = fgMedia.begin(); it != fgMedia.end(); it++) {
        if (*it == blobs[blobID].mediaLink) {
            vid = *it;
            break;
        }
    }
    // update position
    if (vid != NULL) {
        // cout << "blob " << blobID << " \t\tfound vid" << endl;
        (*vid).setDisplay( offsetX + blobs[blobID].position.x * mapSiteW, offsetY + blobs[blobID].position.y *mapSiteH, true);
    }
}

void planeApp::blobUnlink(int & blobID) {
    // making sure, a blob goes to die and untethers all connections
    for (vector<ofPtr<mediaElement> >::iterator it = fgMedia.begin(); it != fgMedia.end(); it++) {
        if (*it == blobs[blobID].mediaLink) {
            cout << "unlinked blob " << blobID << endl;
            fgMedia.erase(it);
            break;
        }
    }
}

void planeApp::nextSegment(int direction){

    int oldScene = scene;
    fgMedia.clear();   // delete all foreground videos

    segment+=direction;
    segmentStart = ofGetUnixTime();

    if(scene == -1 || segment >= scenes[scene].segments) {
        scene++;
        segment = 0;
        cout << "scene " << scene << " / " << scenes.size() << endl;
        if(scene >= scenes.size()) {
            cout << "last scene" << endl;
            scene = 0;
            globalStart = ofGetUnixTime();
        }
    } else if (segment < 0){
        scene--;
        if(scene < 0){
            scene = scenes.size()-1;
        }
        segment = scenes[scene].segments -1;
    }

    // make new blob connections
    for(std::map<int, Blob>::iterator it = blobs.begin(); it != blobs.end(); ++it){
        Blob* b = &it->second;
        ofNotifyEvent(b->onCreate,b->id,b);
    }

    if (oldScene != scene) {
        // reset all BGvideos of the scene
        for (vector<videoElement>::iterator it = bgVideos[scene].begin() ; it != bgVideos[scene].end(); ++it) {
            videoElement* v = &(*it);
            v->reset();
        }
    }

    cout << "segment " << scenes[scene].instructions[segment] << endl;

}

// check for incoming OSC messages
// add new blobs, or update blobs with new data
//--------------------------------------------------------------
void planeApp::receiveOsc(){

	// check for waiting messages
	while(receiver.hasWaitingMessages()){

		// get the next message
		ofxOscMessage m;
		receiver.getNextMessage(&m);

		if(m.getAddress() == "/blobserver/startFrame"){
//			cout << "\n";
		} else if(m.getAddress() == "/blobserver/endFrame"){

		} else if(m.getAddress() == "/blobserver/bgsubtractor"){

		} else if(m.getAddress() == "/blobserver/stitch"){

		} else if(m.getAddress() == "/blobserver/nop"){

		} else if(m.getAddress() == "/blobserver/hog"){
			// parse incoming elements:  iiiffii: id x y vx vy age lost
			int blobid = m.getArgAsInt32(0);
			int posx = m.getArgAsInt32(1);
			int posy = m.getArgAsInt32(2);
			float velx = m.getArgAsFloat(3);
			float vely = m.getArgAsFloat(4);
			int age = m.getArgAsInt32(5);
			int lost = m.getArgAsInt32(6);

			std::map<int,Blob>::iterator iter = blobs.find(blobid);
			if( iter != blobs.end() ) {		// blob instance already exists

			} else {					// create new blob instance
				blobs[blobid].id = blobid;
                blobs[blobid].perspectiveMat = &this->perspectiveMat;
                // add Events, so blobs can report back to planeApp
                ofAddListener( blobs[blobid].onFreeze, this, &planeApp::blobOnFreeze );
                ofAddListener( blobs[blobid].unFreeze, this, &planeApp::blobUnFreeze );
                ofAddListener( blobs[blobid].overFreeze, this, &planeApp::blobOverFreeze );
                ofAddListener( blobs[blobid].prepareToDie, this, &planeApp::blobUnlink );
                ofAddListener( blobs[blobid].onCreate, this, &planeApp::blobOnCreate );
                ofAddListener( blobs[blobid].updatePosition, this, &planeApp::videoFollowBlob );
                ofNotifyEvent( blobs[blobid].onCreate, blobid, &blobs[blobid] );
			}
			// update blob with new values
			Blob* b = &blobs.find(blobid)->second;
            b->follow(posx + blobW/2.0, posy + blobH/2.0, blobserverW, blobserverH, edgeMargin);
            b->setVelocity(velx, vely);
            b->analyze(freezeMinVel, freezeMinTime, freezeMaxTime, movingThr);    //
            b->age = age;
            b->lostDuration = lost;
//			cout << "blob " << blobid << "   " << posx << "|" << posy << "   - lifetime: " << b->lifetime << "\n";
		}
		else{
			// unrecognized message
		}

	}}

//--------------------------------------------------------------
void planeApp::draw(){
    ofBackground(0,50,150);
    // ofBackground(0);
    int offsx = 10; int offsy = 10;

    ofFill(); ofSetColor(255);
    ofDrawBitmapString(ofToString(ofGetFrameRate()), offsx, offsy);

    offsy += 20;

    this->drawRawData(offsx, offsy, 0.7);

    offsy += 220 + 10;
    this->drawTopDown(offsx, offsy, 0.5, drawBlobDetail);

    offsy = 10;
    offsx += 380;
    this->drawAnalysis(offsx, offsy, 0.35);

    offsy = 10;
    offsx += 390;
    this->drawScreen(offsx, offsy, 0.35);

    offsx += 400;
    this->drawControlInfo(offsx, offsy);

    gui.draw();
}


//--------------------------------------------------------------
void planeApp::drawScreen(int x, int y, float scale){


    // black frame first
    ofFill(); ofSetColor(0);
    ofRect(x,y,projectionW*scale,projectionH*scale);
    ofNoFill(); ofSetColor(255);
    ofRect(x,y,projectionW*scale,projectionH*scale);

    // ofEnableAlphaBlending();
    ofEnableBlendMode(OF_BLENDMODE_ADD);

    // background videos
    for (vector<videoElement>::iterator it = bgVideos[scene].begin() ; it != bgVideos[scene].end(); ++it) {
        videoElement* v = &(*it);
        v->draw(x, y, scale);
    }

    for (vector<ofPtr<mediaElement> >::iterator it = fgMedia.begin(); it != fgMedia.end(); ++it) {
        (**it).draw(x, y, scale);
    }

    // extra things to draw? 
    if (scene==1) {
        if (segment==2 || segment==3) {

            for(std::map<int, Blob>::iterator it = blobs.begin(); it != blobs.end(); ++it){
                Blob* b = &it->second;

                if (b->movingMean) {
                    for(std::map<int, Neighbor>::iterator iter = b->neighbors.begin(); iter != b->neighbors.end(); ++iter){
                        Neighbor* nb = &iter->second;
                        if (nb->steadyDistance && blobs[nb->id].movingMean) {
                            // draw a line between blob and steady neighbor
                            ofNoFill(); ofSetColor(255);
                            float p1x = x + (offsetX + b->position.x * mapSiteW) * scale;
                            float p1y = y + (offsetY + b->position.y * mapSiteH) * scale;
                            float p2x = x + (offsetX + blobs[nb->id].position.x * mapSiteW) * scale;
                            float p2y = y + (offsetY + blobs[nb->id].position.y * mapSiteH) * scale;
                            ofLine( p1x, p1y, p2x, p2y );
                            // ofLine(x + b->position.x*scale, y + b->position.y*scale, x + blobs[nb->id].position.x*scale, y + blobs[nb->id].position.y*scale);
                        }
                    }
                }
            }

        }
    }
    ofDisableBlendMode();
    // ofDisableAlphaBlending();
}

//--------------------------------------------------------------
void planeApp::drawAnalysis(int x, int y, float scale){

    // frame first
    ofNoFill(); ofSetColor(255);
    ofRect(x,y,projectionW*scale,projectionH*scale);

    // display  instructions big
    string instruction = scenes[scene].instructions[segment];
    ofFill(); ofSetColor(255);
    font.drawString(instruction, x+20, y+projectionH*scale*0.1);

    if (scene==1) {
        if( segment==0 || segment==1 ) {

            // STAND STILL
            // draw circle for each blob, and display frozen, frozentimer
            int bx = x + 100; int by = y + 250;
            ofFill();
            for(std::map<int, Blob>::iterator it = blobs.begin(); it != blobs.end(); ++it){
                Blob* b = &it->second;
//                int add = b.frozenTimer < 10 ? b.frozenTimer*5 : 50;
//                if(b.frozen) ofSetColor(200+add,0,200+add); else ofSetColor(100);
                if(b->properFreeze) {
                    if(segment==1 && b->frozenTimer > freezeMaxTime) {
                        ofSetColor(50);
                    } else ofSetColor(255);
                } else if(b->frozen) ofSetColor(100);
                else ofSetColor(50);

                ofCircle(bx, by, 50);

                string textStr = "id\t: " + ofToString(b->id);
                textStr += "\nvel\t: " + ofToString(b->vel, 2);
                textStr += "\nfrozen\t: "+ ofToString(b->frozen ? "true" : "false");
                textStr += "\nproperFreeze : "+ ofToString(b->properFreeze ? "true" : "false");
                textStr += "\noverFrozen : "+ ofToString(b->overFrozen ? "true" : "false");
                textStr += "\nfrozenTimer : " + ofToString(b->frozenTimer);
                ofDrawBitmapStringHighlight(textStr, bx+70, by -30);

                by += 110;
            }

        } else if( segment==2 || segment==3 ) {

            // KEEP THE DISTANCE
            int bx = x + 100; int by = y + 250;
            ofFill();
            for(std::map<int, Blob>::iterator it = blobs.begin(); it != blobs.end(); ++it){
                Blob* b = &it->second;

                ofSetColor(255);

                ofCircle(bx, by, 50);

                string textStr = "id\t: " + ofToString(b->id);
                textStr += "\nneighbors : "+ ofToString(b->neighbors.size());
                textStr += "\nmovingMean : " + ofToString(b->movingMean ? "true" : "false");
                ofDrawBitmapStringHighlight(textStr, bx+70, by -40);

                for(std::map<int, Neighbor>::iterator iter = b->neighbors.begin(); iter != b->neighbors.end(); ++iter){
                    Neighbor* nb = &iter->second;
                    if (nb->steadyDistance) ofSetColor(255); else ofSetColor(0);
                    ofDrawBitmapString(ofToString(nb->id), bx+70, by+20);
                    ofRect(bx+100, by+20, nb->distance[0]*0.2, -10);
                    by += 20;
                }

                by += 110;
            }
        }
    } else if (scene==3) {
        if (segment==1 || segment==2) {

            // HOP
            int bx = x + 100; int by = y + 250;
            ofFill();
            for(std::map<int, Blob>::iterator it = blobs.begin(); it != blobs.end(); ++it){
                Blob* b = &it->second;


                if(b->lostDuration > 0 && b->lostDuration < hopLength && !b->onEdge) ofSetColor(255);
                else ofSetColor(100);

                ofCircle(bx, by, 40);

                string textStr = "id\t: " + ofToString(b->id);
                textStr += "\nonEdge\t: " + ofToString(b->onEdge ? "true" : "false");
                textStr += "\nlost\t: "+ ofToString(b->lostDuration);
                ofDrawBitmapStringHighlight(textStr, bx+70, by -20);

                by += 90;
            }

        }

    }



}

//  FOR TESTING WARPING MATH
//    // testBlob - draw raw history
//    ofSetColor(200,200,200);
//    for (vector<TimedPoint>::iterator it = testBlob.rawHistory.begin() ; it != testBlob.rawHistory.end(); ++it) {
//        ofCircle( offsx + (*it).point.x, offsy + (*it).point.y, 2);
//    }
//
//    // testBlob - draw transformed history
//    ofSetColor(200,0,0);
//    for (vector<TimedPoint>::iterator it = testBlob.history.begin() ; it != testBlob.history.end(); ++it) {
//        ofCircle( offsx + (*it).point.x, offsy + (*it).point.y, 2);
//    }


void planeApp::drawControlInfo(int x, int y){
    ofFill(); ofSetColor(255);

    string instruction = scenes[scene].instructions[segment];
    ofDrawBitmapString("SCENE ID\t" + ofToString(scene) +
                       "\nSCENE NAME\t" + scenes[scene].name +
                       "\nSEGMENT\t\t" + ofToString(segment) +
                       "\n\n" + instruction +
                       "\n\nGLOBAL TIME\t" + ofToString(masterClock) +
                       "\nSEGMENT TIME\t" + ofToString(segmentClock), x+3, y+10 );
}


// draw raw data / small display
//--------------------------------------------------------------
void planeApp::drawRawData(int x, int y, float scale){

//    int blobserverW = 192;
//    int blobserverH = 144;

    // frame first
    ofNoFill(); ofSetColor(255);
    ofRect(x,y,blobserverW*scale,blobserverH*scale);

    // write information
    string rawInfo = "port: \t" + ofToString(PORT);
    rawInfo += "\nframerate: \t" + ofToString(28);
    rawInfo += "\nexposure: \t" + ofToString(0.4333,2);
    ofDrawBitmapStringHighlight(rawInfo, x + 3, y + blobserverH*scale + 15);

    // draw frame for each blob. blobserver frame size = 64 x 128 px
    for(std::map<int, Blob>::iterator it = blobs.begin(); it != blobs.end(); ++it){
        Blob* b = &it->second;
        ofRect( x + b->_rawPos.x*scale - blobW*scale/2.0, y + b->_rawPos.y*scale - blobH*scale/2.0, blobW*scale, blobH*scale);
    }

}

//--------------------------------------------------------------
void planeApp::drawTopDown(int x, int y, float scale, bool detailed) {

    // frame
    ofNoFill(); ofSetColor(255);
    ofRect(x,y,siteW*scale,siteH*scale);

    // write information
    string basicInfo = "blobs: " + ofToString(blobs.size());
    ofDrawBitmapStringHighlight(basicInfo, x + 3, y +siteH*scale+ 15);

    // draw history
    ofNoFill(); ofSetColor(150);
    for(std::map<int, Blob>::iterator it = blobs.begin(); it != blobs.end(); ++it){
        Blob* b = &it->second;
        ofBeginShape();
        for (vector<TimedPoint>::iterator it = b->history.begin() ; it != b->history.end(); ++it) {
            ofVertex( x + (*it).point.x*scale, y + (*it).point.y*scale );
        }
        ofEndShape();
    }

    // draw blobs as circles
    ofFill();
    for(std::map<int, Blob>::iterator it = blobs.begin(); it != blobs.end(); ++it){
        ofSetColor(255);
        Blob* b = &it->second;
        int bx = x + b->position.x*scale;
        int by = y + b->position.y*scale;
        ofCircle( bx, by, 20);

        // display blob details
        if(detailed) {
            string textStr = "id: " + ofToString(b->id);
            textStr += "\nvel:" + ofToString(b->velocity.x, 1) + "/" + ofToString(b->velocity.y, 1);
            textStr += "\nage: "+ofToString(b->age);
            textStr += "\nlost: " + ofToString(b->lostDuration);
            ofDrawBitmapStringHighlight(textStr, bx, by + 10);
        } else {
            ofFill(); ofSetColor(0);
            ofDrawBitmapString( ofToString(b->id, 4, '0'), bx-15, by+5);
        }

    }
}


void planeApp::exit() {
    // do some destructing here
    cout << "goodbye" << endl;
}


//--------------------------------------------------------------
void planeApp::keyPressed(int key){

}

//--------------------------------------------------------------
void planeApp::keyReleased(int key){

    if( key == 'd' ) {
        drawBlobDetail = !drawBlobDetail;
    }

    if (key == OF_KEY_LEFT){
        nextSegment(-1);
    } else if (key == OF_KEY_RIGHT){
        nextSegment(1);
    }

    if(key == 's') {
		gui.saveToFile("settings.xml");
	}
	if(key == 'l') {
		gui.loadFromFile("settings.xml");
	}
}

//--------------------------------------------------------------
void planeApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void planeApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void planeApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void planeApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void planeApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void planeApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void planeApp::dragEvent(ofDragInfo dragInfo){

}
