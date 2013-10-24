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

    fullscreen = false;
    success = false;
    successCnt = 0;
	drawBlobDetail = false;
    transition = false;
    moveOn = false;
    sceneChange = false;
    segmentChange = 1;
    flash = false;
    flashCnt = 0;
    flashMax = 20;

    projectionW = 1080;
    projectionH = 1920;
    blobserverW = 320;
    blobserverH = 240;
    blobW = 64;
    blobH = 128;

    siteW.addListener(this,&planeApp::recalculatePerspective);
	siteH.addListener(this,&planeApp::recalculatePerspective);


	gui.setup("HUMANS AND PLANETS", "planets01.xml", 1084,250);
    gui.setDefaultBackgroundColor( ofColor(0,0,50) );
    gui.add(autoplay.setup("autoplay", false)); 
    gui.add(flashColor.set("Transition Flash Color",ofColor(255,200,100),ofColor(0,0),ofColor(255,255)));

    paramBasic.setName("Dimension"); 
    paramBasic.add(siteW.set( "Site Width", 500, 0, 1000));
    paramBasic.add(siteH.set( "Site Length", 700, 0, 1000));
    paramBasic.add(mapSiteW.set( "Map Width", 2, 0, 4));
    paramBasic.add(mapSiteH.set( "Map Height", 2, 0, 4));
    paramBasic.add(offsetX.set( "Offset X", 0, -500, 500));
    paramBasic.add(offsetY.set( "Offset Y", 0, -500, 500));
    gui.add(paramBasic);

    // paramTiming.setName("Timing");
    // gui.add(paramTiming);

    paramSc1.setName("Sc1 Stars");
    paramSc1.add(freezeMaxVel.set( "Freeze MaxVel",0.1, 0, 1.0 ));
    paramSc1.add(freezeMinTime.set( "Freeze MinTime",2, 0, 5 ));
    paramSc1.add(freezeMaxTime.set( "Freeze MaxTime",10, 0, 30 ));
    paramSc1.add(keepDistanceThr.set( "Dist StdDev", 10, 0, 20));
    paramSc1.add(movingThr.set( "Movement Thr", 0.1, 0, 2));
    paramSc1.add(newStarMax.set( "NewStar Max", 30, 1, 40));
    paramSc1.add(newStarBonus.set( "Bonus every", 5, 1, 40));
    gui.add(paramSc1);


    // paramSc2.setName("Sc2 Revolutions");
    // gui.add(paramSc2);

    paramSc3.setName("Sc3 Sun");
    paramSc3.add(edgeMargin.set( "Edge Margin", 50, 0, 150));
    gui.add(paramSc3);

    // paramSc4.setName("Sc4 Alignment");
    // gui.add(paramSc4);

    // paramSc5.setName("Sc5 Combustion");
    // gui.add(paramSc5);


	gui.setSize(200,500);
    // gui.loadFromFile("settings.xml");

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
    idle.instructions[0] = "Come closer";
    idle.length[0] = -1;
    scenes[n] = idle;
 
    // bgVideos[n].push_back(videoElement("video/1_idle/IDLE_MODE_5-animation.mov"));
    bgVideos[n].push_back(ofPtr<mediaElement>( new videoElement("video/1_idle/IDLE_MODE_5-animation.mov")));
    ofAddListener( (*bgVideos[n][0]).fadeOutEnd, this, &planeApp::bgMediaFadedOut );
    ofAddListener( (*bgVideos[n][0]).fadeInEnd, this, &planeApp::bgMediaFadedIn );
    (*bgVideos[n][0]).outroTransformation = &mediaElement::finishFast; 

    n++;

    sceneInfo stars;
    stars.name = "Star Links";
    stars.no = n;
    stars.segments = 4;
    stars.instructions[0] = "Stand still";
    stars.length[0] = 20;
    stars.instructions[1] = "Try new spots \nto light up \nmore stars.";
    stars.length[1] = 40;
    stars.instructions[2] = "Walk with \nsomeone. \nKeep the \nsame distance \nbetween you.";
    stars.length[2] = 45;
    stars.instructions[3] = "Walk with \nsomeone. \nMake eye \ncontact. \nKeep the \ndistance.";
    stars.length[3] = 60;
    scenes[n] = stars;

    // bgVideos[n].push_back(videoElement("video/2_stars/BACKGROUND 1 loop-QTAnimation.mov"));
    bgVideos[n].push_back(ofPtr<mediaElement>( new videoElement("video/2_stars/BACKGROUND 1 loop-QTAnimation.mov")));
    ofAddListener( (*bgVideos[n][bgVideos[n].size()-1]).fadeOutEnd, this, &planeApp::bgMediaFadedOut );
    ofAddListener( (*bgVideos[n][0]).fadeInEnd, this, &planeApp::bgMediaFadedIn );

    n++;

    sceneInfo revolution;
    revolution.name = "Revolution";
    revolution.no = n;
    revolution.segments = 2;
    revolution.instructions[0] = "Take someone's \nhand. \nSpin and lean \nout as far as \npossible.";
    revolution.length[0] = 20;
    revolution.instructions[1] = "Let go...";
    revolution.length[1] = 20;
    scenes[n] = revolution;

    // bgVideos[n].push_back(videoElement("video/2_stars/BACKGROUND 1 loop-QTAnimation.mov"));
    bgVideos[n].push_back(ofPtr<mediaElement>( new videoElement("video/2_stars/BACKGROUND 1 loop-QTAnimation.mov")));
    ofAddListener( (*bgVideos[n][bgVideos[n].size()-1]).fadeOutEnd, this, &planeApp::bgMediaFadedOut );
    ofAddListener( (*bgVideos[n][0]).fadeInEnd, this, &planeApp::bgMediaFadedIn );

    n++;

    sceneInfo sun;
    sun.name = "Explosion";
    sun.no = n;
    sun.segments = 6;
    sun.instructions[0] = "Stand on one leg.";
    sun.length[0] = 25;
    sun.instructions[1] = "Hope from one \nspot to the other.";
    sun.length[1] = 20;
    sun.instructions[2] = "Hope from one \nspot to the other.\nEveryone in unison.";
    sun.length[2] = 20;
    sun.instructions[3] = "FREEZE!";
    sun.length[3] = 20;
    sun.instructions[4] = "Run in every \ndirection at once.";
    sun.length[4] = 40;
    sun.instructions[5] = "FREEZE!";
    sun.length[5] = 13;
    scenes[n] = sun;

    // bgVideos[n].push_back(videoElement("video/4_sun/SUN_8-ASSET-animation.mov"));
    // bgVideos[n][bgVideos[n].size()-1].setDisplay(projectionW/2,projectionH/2, true);

    bgVideos[n].push_back(ofPtr<mediaElement>( new videoElement("video/2_stars/BACKGROUND 1 loop-QTAnimation.mov")));
    ofAddListener( (*bgVideos[n][bgVideos[n].size()-1]).fadeOutEnd, this, &planeApp::bgMediaFadedOut );
    ofAddListener( (*bgVideos[n][0]).fadeInEnd, this, &planeApp::bgMediaFadedIn );

    // bgVideos[n].push_back(ofPtr<mediaElement>( new videoElement("video/4_sun/SUN_8-ASSET-animation.mov")));
    // (*bgVideos[n][bgVideos[n].size()-1]).setDisplay(projectionW/2,projectionH/2, true);
    // ofAddListener( (*bgVideos[n][bgVideos[n].size()-1]).fadeOutEnd, this, &planeApp::bgMediaFadedOut );
    // ofAddListener( (*bgVideos[n][0]).fadeInEnd, this, &planeApp::bgMediaFadedIn );

    n++;

    sceneInfo eclipse;
    eclipse.name = "Alignment";
    eclipse.no = n;
    eclipse.segments = 4;
    eclipse.instructions[0] = "Now align \nyourself in \nfront of me.\nAnd follow me.";
    eclipse.length[0] = 40;
    eclipse.instructions[1] = "Step out \nof the line.\nStep into \nthe line.";
    eclipse.length[1] = 20;
    eclipse.instructions[2] = "Disperse very \nslowly towards \nthe edges.";
    eclipse.length[2] = 15;
    scenes[n] = eclipse;

    // bgVideos[n].push_back(videoElement("video/2_stars/BACKGROUND 1 loop-QTAnimation.mov"));
    bgVideos[n].push_back(ofPtr<mediaElement>( new videoElement("video/2_stars/BACKGROUND 1 loop-QTAnimation.mov")));
    ofAddListener( (*bgVideos[n][bgVideos[n].size()-1]).fadeOutEnd, this, &planeApp::bgMediaFadedOut );
    ofAddListener( (*bgVideos[n][0]).fadeInEnd, this, &planeApp::bgMediaFadedIn );

    n++;

    sceneInfo shooting;
    shooting.name = "Combustion";
    shooting.no = n;
    shooting.segments = 4;
    shooting.instructions[0] = "Move like a \nshooting star.";
    shooting.length[0] = 25;
    shooting.instructions[1] = "Drop to the \nground!";
    shooting.length[1] = 15;
    shooting.instructions[2] = "Exhale.\nLook at the sky.";
    shooting.length[2] = 15;
    shooting.instructions[3] = "Stand up";
    shooting.length[3] = 10;
    shooting.instructions[4] = "Thank you";
    shooting.length[4] = 5;
    scenes[n] = shooting;

    // bgVideos[n].push_back(videoElement("video/2_stars/BACKGROUND 1 loop-QTAnimation.mov"));
    bgVideos[n].push_back(ofPtr<mediaElement>( new videoElement("video/2_stars/BACKGROUND 1 loop-QTAnimation.mov")));
    ofAddListener( (*bgVideos[n][bgVideos[n].size()-1]).fadeOutEnd, this, &planeApp::bgMediaFadedOut );
    ofAddListener( (*bgVideos[n][0]).fadeInEnd, this, &planeApp::bgMediaFadedIn );

    cout << "there are " << scenes.size() << " scenes" << endl;

    sceneChange = true;
    nextSegment(1);

}

//--------------------------------------------------------------
void planeApp::update(){

    this->receiveOsc();

	// BLOB CLEANUP
    std::map<int,Blob>::iterator it = blobs.begin();
    while (it != blobs.end()) {
        Blob* b = &it->second;
    	b->update();
    	if( !b->isAlive() ) {
//            cout << "erase blob " << b->id << endl;
            ofNotifyEvent(b->prepareToDie,b->id,b);
    		blobs.erase(it++);
            blobCountChange();
    	} else {
    		++it;
    	}
    }

    // ANALYSIS
    std::map<int, ofPoint> blobPositions;
    for(std::map<int, Blob>::iterator it = blobs.begin(); it != blobs.end(); ++it){
        Blob* b = &it->second;
        blobPositions[b->id] = b->position;
    }
    for(std::map<int, Blob>::iterator it = blobs.begin(); it != blobs.end(); ++it){
        Blob* b = &it->second;
        b->analyzeNeighbors(blobPositions, keepDistanceThr);
    }

    // SCHEDULING
    masterClock = ofGetUnixTime() - globalStart;
    segmentClock = ofGetUnixTime() - segmentStart;
    if (flash) {
        flashCnt++;
        if (flashCnt > flashMax) {
            cout << "flash done" << endl;
            flashCnt = 0;
            flash = false;
            beginSegment();
        }
    }
    // TIME TRIGGERS
    if (autoplay && scenes[scene].length[segment] > 0 && segmentClock >= scenes[scene].length[segment]) {
        endSegment(1);
    }
    // triggered by the end of fading out the bg
    if (moveOn) {   
        nextSegment(segmentChange);
        moveOn = false;
    }

    // TRANSITIONING BETWEEN SEGMENT RELATED ACTION
    if (scene==0) {
        // fade out idle-mode video, connect fade-End to transition to next Segment
        if(success && !transition) {
            cout << "scene 0 success" << endl;
            endSegment(1);
        }
    } else if (scene==1) {
        if (segment==1 && successCnt > newStarMax) {
            cout << "scene 1 segment 1 success" << endl;
            endSegment(1);
        }
    } else if (scene==3) {
        if (segment==4 || segment==6 ) {
            // FREEZE!
            if(success && !transition) {
                cout << "FREEZE success" << endl;
                endSegment(1);
            }
        }
    }

    // VIDEO UPDATES
    for (vector<ofPtr<mediaElement> >::iterator it = bgVideos[scene].begin() ; it != bgVideos[scene].end(); ++it) {
        (**it).update();
    }

    vector<ofPtr<mediaElement> >::iterator iter = fgMedia.begin();
    while (iter != fgMedia.end()) {
        (**iter).update();
        if ((**iter).dead) {
            cout << "delete video " << (**iter).file << "  size " << fgMedia.size() << endl;
            iter = fgMedia.erase(iter);
        } else {
            ++iter;
        }
    }

    // animation related
    if (scene==3 && segment==5) {
        if (ofRandom(100)<2) (*fgMedia[0]).bounce();
    }

}

void planeApp::bgMediaFadedOut(int & trans) {
    cout << "bgMediaFadedOut" << endl;
    // set moveOn to true, instead of calling nextSegment()
    // to avoid conflicting threading
    moveOn = true;
}

// trans -1 : trigger first fade from main-loop and not from faded-event
void planeApp::fgMediaFadedOut(int & trans) {
    cout << "fgMediaFadedOut  " << trans << "  fgmedia size = " << fgMedia.size() << endl;
    
    if (fgMedia.size()>0) {
        
        if (trans == -1) {
            ofAddListener( (*fgMedia.back()).fadeOutEnd, this, &planeApp::fgMediaFadedOut );
            cout << "call outroTransformation() on fgMedia[" << (fgMedia.size()-1) << "]" << endl;
            ((*fgMedia.back()).*((*fgMedia.back()).outroTransformation))();

        } else {
            // delete last fgMedia object
            (*fgMedia.back()).dead = true;

            if (fgMedia.size()>1) {
                // fade new last fgMedia object
                ofAddListener( (*fgMedia[fgMedia.size()-2]).fadeOutEnd, this, &planeApp::fgMediaFadedOut );
                cout << "call outroTransformation() on fgMedia[" << (fgMedia.size()-2) << "]" << endl;
                ((*fgMedia[fgMedia.size()-2]).*((*fgMedia[fgMedia.size()-2]).outroTransformation))();
            } else {
                // fade BG now
                if (scene==0) {
                    ((*bgVideos[scene][0]).*((*bgVideos[scene][0]).outroTransformation))();
                } else {
                    (*bgVideos[scene][0]).fadeOut();
                }
            }
        }
    } else {
        // fade BG now
        if (scene==0) {
            ((*bgVideos[scene][0]).*((*bgVideos[scene][0]).outroTransformation))();
        } else {
            (*bgVideos[scene][0]).fadeOut();
        }
    }
}

void planeApp::fgMediaFadedIn(int & trans) {
    cout << "fgMediaFadedIn  " << trans << endl;
    if (fgMedia.size() > 0) {
        for (vector<ofPtr<mediaElement> >::iterator it = fgMedia.begin(); it != fgMedia.end(); it++) {
            if (!(**it).visible) {
                cout << "fgMediaFadedIn   " << (**it).file << endl;
                ofAddListener( (**it).fadeInEnd, this, &planeApp::fgMediaFadedIn );
                ((**it).*((**it).introTransformation))();
            }
        }
    }
}

void planeApp::bgMediaFadedIn(int & trans) {
    cout << "bgMediaFadedIn" << endl;
    transition = false;

    // make new blob connections, to ensure blobs are connected to video elements, if necessary
    for(std::map<int, Blob>::iterator it = blobs.begin(); it != blobs.end(); ++it){
        cout << "new blob connection " << endl;
        Blob* b = &it->second;
        ofNotifyEvent(b->onCreate,b->id,b);
    }

    for (vector<ofPtr<mediaElement> >::iterator it = fgMedia.begin(); it != fgMedia.end(); it++) {
        ((**it).*((**it).introTransformation))();
    }
    // int tmp = -1;
    // fgMediaFadedIn(tmp);
}

void planeApp::blobOnLost(int & blobID) {
    // cout << "BLOB " << blobID << " just got lost" << endl;
    if (!transition) {
        if (scene==3) {
            // SUN explosions
            if (segment==1 || segment==2) {
                int randomShooter = ofRandom(6) + 1;
                // string newVideoName = "video/4_sun/SUN_explosion-" + ofToString(randomShooter,2,'0') + "-animation.mov";
                string newVideoName = "video/4_sun/SUN_explosion-" + ofToString(randomShooter,2,'0') + "-animation.mp4";
                // newVideoName = "video/4_sun/SUN_explosion-01-H264-10mbps.mp4";
                // newVideoName = "video/4_sun/SUN_explosion-01-photoJPEG.mov";
                newVideoName = "video/4_sun/SUN_explosion-01-animation.mov";
                fgMedia.push_back(ofPtr<mediaElement>( new videoElement(newVideoName)));
                (*fgMedia[fgMedia.size()-1]).setDisplay(projectionW/2 + ofRandom(-200,200), projectionH/2 + ofRandom(-200,200), true);
                (*fgMedia[fgMedia.size()-1]).autoDestroy(true);
                (*fgMedia[fgMedia.size()-1]).reset();
                if (segment==2) (*fgMedia[0]).bounce(); // sun video = [0]
            }
        } else if (scene==5) {
            // SHOOTING STARS
            if (segment==0) {
                int randomShooter = ofRandom(6) + 1;
                fgMedia.push_back(ofPtr<mediaElement>( new videoElement("video/6_shooting/SSTAR_0" + ofToString(randomShooter) + "-H264-10mbps.mp4")));
                (*fgMedia[fgMedia.size()-1]).setDisplay(ofRandom(projectionW-100), ofRandom(projectionH-100), true);
                (*fgMedia[fgMedia.size()-1]).moveAcross( 45.f, projectionW, projectionH, true);
                (*fgMedia[fgMedia.size()-1]).reset();
            } else if (segment==1) {
                float randdeg = ofRandom(-5.f, 5.f);
                for (int i=0; i<10; i++) {
                    int randomShooter = ofRandom(6) + 1;
                    fgMedia.push_back(ofPtr<mediaElement>( new videoElement("video/6_shooting/SSTAR_0" + ofToString(randomShooter) + "-H264-10mbps.mp4")));
                    (*fgMedia[fgMedia.size()-1]).setDisplay(ofRandom(projectionW-100), ofRandom(projectionH-100), true);
                    (*fgMedia[fgMedia.size()-1]).moveAcross( randdeg, 45.f, projectionW, true);
                    (*fgMedia[fgMedia.size()-1]).reset();
                }
            }
        }
    }
}


void planeApp::blobOnFreeze(int & blobID) {
//    cout << "BLOB " << blobID << " froze!" << endl;
    if (!transition) {
        if (scene==1) {
            if (segment==0 || segment==1) {
                // STAND STILL
                // create new video element
    //            cout << "blob " << blobID << " \t\tfreeze ! " << endl;
                fgMedia.push_back(ofPtr<mediaElement>( new videoElement("video/2_stars/INDIV STAR 2 loop-H264-10mbps.mp4")));
                blobs[blobID].mediaLink = fgMedia[fgMedia.size()-1];
                (*fgMedia[fgMedia.size()-1]).setDisplay(ofRandom(projectionW-100), ofRandom(projectionH-100));
                (*fgMedia[fgMedia.size()-1]).reset();
                (*fgMedia[fgMedia.size()-1]).outroTransformation = &mediaElement::scaleAway;
                if (segment==1) {
                    successCnt++;
                    if (successCnt%newStarBonus == 0) {
                        string bonusVideo = "video/2_stars/HAND animation-QTAnimation.mov";
                        fgMedia.push_back(ofPtr<mediaElement>( new videoElement(bonusVideo) ));
                        (*fgMedia[fgMedia.size()-1]).setDisplay(ofRandom(projectionW-600), ofRandom(projectionH-800));
                        (*fgMedia[fgMedia.size()-1]).reset(true);
                        (*fgMedia[fgMedia.size()-1]).autoDestroy(true);
                    }
                }
            }
        } else if (scene==3) {
            if ((segment==4 || segment==6) && !success) {
                // FREEZE!
                // check if all blobs are frozen
                // frozen
                bool allFrozen = true;
                for(std::map<int, Blob>::iterator it = blobs.begin(); it != blobs.end(); ++it){
                    Blob* b = &it->second;
                    if (!b->frozen) allFrozen = false;
                }
                if (allFrozen) success = true;
            }
        }
    }
}
void planeApp::blobUnFreeze(int & blobID) {

    if (!transition) {
        if (scene==1) {
            if (segment==0 || segment==1) {
    //            cout << "BLOB " << blobID << " moved again!  mediaLink = " << blobs[blobID].mediaLink << endl;
                // STAND STILL
                this->blobUnlink(blobID);
            }
        }
    }
}

void planeApp::blobOverFreeze(int & blobID) {
    if (!transition) {
        if (scene==1) {
            if (segment==1) {
                // no more video, been frozen too long!
                this->blobUnlink(blobID);
            }
        }
    }
}

void planeApp::blobOnCreate(int & blobID) {
   
    if (!transition) {
        // first clean up, all related blob settings
        this->blobUnlink(blobID);
        blobs[blobID].videoTrace = false;
        blobs[blobID].mediaLink = ofPtr<mediaElement>();  // TODO how to release ofPtr ? 
        

        cout << "BLOB " << blobID << " blobOnCreate!" << endl;

        // then assign the appropriate ones
        if (scene==0) {
            // COME CLOSER, recognition of people == time to move on
            if (!success) success = true;
        } else if (scene==1) {
            if (segment==2 || segment==3) {
                // KEEP THE DISTANCE
                // create new video element
    //            cout << "blob " << blobID << " \t\tfreeze ! " << endl;
                fgMedia.push_back(ofPtr<mediaElement>( new videoElement("video/2_stars/INDIV STAR 2 loop-H264-10mbps.mp4")));
                blobs[blobID].mediaLink = fgMedia[fgMedia.size()-1];
                blobs[blobID].videoTrace = true;
                (*fgMedia[fgMedia.size()-1]).setDisplay(0, 0, 200, 200, true);
                // (*fgMedia[fgMedia.size()-1]).setDisplay(ofRandom(projectionW-100), ofRandom(projectionH-100));
                (*fgMedia[fgMedia.size()-1]).reset();
                (*fgMedia[fgMedia.size()-1]).fadeIn();
                (*fgMedia[fgMedia.size()-1]).outroTransformation = &mediaElement::scaleAway;
            }
        } else if (scene==4) {
            // PLANETS
            int randomPlanet = ofRandom(3) + 18;
            // fgMedia.push_back(ofPtr<mediaElement>( new imageElement("video/5_eclipse/PLANET_" + ofToString(randomPlanet)+".png", 0.5f)));
            fgMedia.push_back(ofPtr<mediaElement>( new videoElement("video/5_eclipse/PLANET_"+ofToString(randomPlanet)+".mov")));
            float x = offsetX + blobs[blobID].position.x * mapSiteW;
            float y = offsetY + blobs[blobID].position.y *mapSiteH;
            cout << "init planet   pos: " << x << "/" << y << endl;
            (*fgMedia[fgMedia.size()-1]).setDisplay(x, y, 500, 500, true);
            // (*fgMedia[fgMedia.size()-1]).introTransformation = &mediaElement::moveInFromSide;
            blobs[blobID].mediaLink = fgMedia[fgMedia.size()-1];
            blobs[blobID].videoTrace = true;
            (*fgMedia[fgMedia.size()-1]).reset();
            (*fgMedia[fgMedia.size()-1]).moveInFromSide(projectionW/2,projectionH/2);
        }

        blobCountChange();
    }
}

// triggered when a blob is added or erased
void planeApp::blobCountChange() {

    if ( scene==2 && fgMedia.size()>=5 ) {
        // REVOLUTION

        // there should only be 5 fgMedia elements
        for (unsigned int i=0; i<fgMedia.size(); i++) {
            (*fgMedia[i]).hide = true;
            if (i < blobs.size()+3) (*fgMedia[i]).hide = false;
            (*fgMedia[i]).rotation = 0;
        }

        // positioning
        switch (blobs.size()+3) {
            case 1:     (*fgMedia[0]).setDisplay(projectionW/2, projectionH/2, true);
                        break;
            case 2:     (*fgMedia[0]).setDisplay(projectionW/2, projectionH/2 - 350, true);
                        (*fgMedia[1]).setDisplay(projectionW/2, projectionH/2 + 350, true);
                        break;
            case 3:     (*fgMedia[0]).setDisplay(projectionW/2, projectionH/2 - 550, true);
                        (*fgMedia[1]).setDisplay(projectionW/2, projectionH/2, true);
                        (*fgMedia[2]).setDisplay(projectionW/2, projectionH/2 + 550, true);
                        break;
            case 4:     (*fgMedia[0]).setDisplay(projectionW/2-100, projectionH/2 - 550, true);
                        (*fgMedia[1]).setDisplay(projectionW/2+100, projectionH/2 - 200, true);
                        (*fgMedia[2]).setDisplay(projectionW/2-100, projectionH/2 + 200, true);
                        (*fgMedia[3]).setDisplay(projectionW/2+100, projectionH/2 + 550, true);
                        break;
            default:
            case 5:     (*fgMedia[0]).setDisplay(projectionW/2, projectionH/2, true);
                        (*fgMedia[1]).setDisplay(projectionW/2-200, projectionH/2 - 350, true);
                        (*fgMedia[1]).rotation = -45;
                        (*fgMedia[2]).setDisplay(projectionW/2+200, projectionH/2 - 350, true);
                        (*fgMedia[2]).rotation = 45;
                        (*fgMedia[3]).setDisplay(projectionW/2-200, projectionH/2 + 350, true);
                        (*fgMedia[3]).rotation = 45;
                        (*fgMedia[4]).setDisplay(projectionW/2+200, projectionH/2 + 350, true);
                        (*fgMedia[4]).rotation = -45;
                        break;
        }
    }

}

void planeApp::videoFollowBlob(int & blobID) {
    // cout << "blob " << blobID << " \t\tvideoFollowBlob" << endl;
    // find videoElement
    if (!transition) {
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
            float x = offsetX + blobs[blobID].position.x * mapSiteW;
            float y = offsetY + blobs[blobID].position.y *mapSiteH;
            if (!(*vid).moveElement) {
                (*vid).setDisplay( x, y, true);
            } else {
                (*vid).goal.set( x, y );
            }
        }
    }
}

void planeApp::blobUnlink(int & blobID) {
    // making sure, a blob goes to die and untethers all connections
    for (vector<ofPtr<mediaElement> >::iterator it = fgMedia.begin(); it != fgMedia.end(); it++) {
        if (*it == blobs[blobID].mediaLink) {
            blobs[blobID].mediaLink = ofPtr<mediaElement>();
            cout << "unlinked blob " << blobID << endl;
            fgMedia.erase(it);
            break;
        }
    }
}

void planeApp::beginSegment() {
    cout << "beginSegment()" << endl;
    // ONLY FADE BG IN, IF SCENE CHANGE HAPPENED
    if (sceneChange) {
        cout << "beginSegment() fade in BG   " << scene << ":" << segment << endl;
        for (vector<ofPtr<mediaElement> >::iterator it = bgVideos[scene].begin() ; it != bgVideos[scene].end(); ++it) {
            (**it).fadeIn();
        }
        transition = true;
    }

}

void planeApp::endSegment(int direction) {
    cout << "endSegment()   " << scene << ":" << segment << endl;
    segmentChange = direction;

    // ONLY DO BG-VIDEO TRANSITIONS IF A SCENE CHANGE IS COMING UP!
    if (segment+segmentChange >= scenes[scene].segments || segment+segmentChange < 0) {
        sceneChange = true;

        int tmp = -1;
        fgMediaFadedOut(tmp);   // TODO clean up, just need to hand back ref. of integer

        transition = true;
    } else {
        // if no BG fading is necessary, simply move on to the next segment
        sceneChange = false;
        moveOn = true;
    }
}

void planeApp::jumpToScene(int s) {
    scene = s;
    segment = 0;
    sceneChange = true;
    transition = true;
    initSegment();
}

void planeApp::nextSegment(int direction) {
    segment+=direction;

    if(scene == -1 || segment >= scenes[scene].segments) {
        scene++;
        segment = 0;
        if(scene >= scenes.size()) {
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


    if (sceneChange) cout << "scene " << scene << ": " << scenes[scene].name << endl;
    cout << "nextSegment() " << scene << ": segment " << segment << ": " << scenes[scene].instructions[segment] << endl;

    initSegment();
}

void planeApp::initSegment(){

    fgMedia.clear();   // delete all foreground videos
    success = false;
    successCnt = 0;
    flash = true;       // 
    segmentStart = ofGetUnixTime();

    // NEW SCENE
    if (sceneChange) {
        // reset all BGvideos of the scene
        for (vector<ofPtr<mediaElement> >::iterator it = bgVideos[scene].begin() ; it != bgVideos[scene].end(); ++it) {
            (**it).reset(false);
        }
    }

    // add FG videos
    if (scene==2) {
        // REVOLUTIONS
        for (int i=0; i<5; i++) {
            string videoFile;
            if (segment==0) {
                int videoPick = i+1;
                videoFile = "video/3_revolution/REV_0"+ofToString(videoPick)+"-animation.mov";
            } else if (segment==1)  {
                int videoPick = (i==4||i==2) ? 4 : i+1;
                videoFile = "video/3_revolution/REV_0"+ofToString(videoPick)+"-out-animation.mov";
            }
            fgMedia.push_back(ofPtr<mediaElement>( new videoElement(videoFile)));
            // fgMedia.push_back(ofPtr<mediaElement>( new videoElement("video/3_revolution/REV_06-animation.mov")));
            (*fgMedia[fgMedia.size()-1]).outroTransformation = &mediaElement::scaleAway;
            if (sceneChange) (*fgMedia[i]).reset(false);
            else (*fgMedia[i]).reset();
        }

        blobCountChange();  // to position and turn on/off videos
    } else if (scene==3) {
        // SUN
        fgMedia.push_back(ofPtr<mediaElement>( new videoElement("video/4_sun/SUN_8-ASSET-animation.mov")));
        (*fgMedia[fgMedia.size()-1]).setDisplay(projectionW/2,projectionH/2, true);
        if (sceneChange) (*fgMedia[fgMedia.size()-1]).introTransformation = &mediaElement::moveInFromTop; 
        (*fgMedia[fgMedia.size()-1]).outroTransformation = &mediaElement::scaleAway;
        if (sceneChange) (*fgMedia[fgMedia.size()-1]).reset(false);
        else (*fgMedia[fgMedia.size()-1]).reset();

    } else if (scene==4) {
        // ECLIPSE. create white/black MAIN PLANET
        fgMedia.push_back(ofPtr<mediaElement>( new mediaElement()));
        (*fgMedia[fgMedia.size()-1]).setDisplay( projectionW/2, projectionH/2, 400, 400 );
        // (*fgMedia[fgMedia.size()-1]).outroTransformation = &mediaElement::scaleAway;
        if (sceneChange) (*fgMedia[fgMedia.size()-1]).reset(false);
        else (*fgMedia[fgMedia.size()-1]).reset();
        (*fgMedia[fgMedia.size()-1]).blend = false;
        if (segment>0) {
            (*fgMedia[fgMedia.size()-1]).clr = ofColor(0, 0, 0);
            (*fgMedia[fgMedia.size()-1]).moveInFromSide(projectionW/2,projectionH/2);
        } else {
            
        }
    }

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
            bool newBlob = false;
			if( iter != blobs.end() ) {		// blob instance already exists

			} else {					// create new blob instance
                newBlob = true;
				blobs[blobid].id = blobid;
                blobs[blobid].perspectiveMat = &this->perspectiveMat;
                // add Events, so blobs can report back to planeApp
                ofAddListener( blobs[blobid].onLost, this, &planeApp::blobOnLost );
                ofAddListener( blobs[blobid].onFreeze, this, &planeApp::blobOnFreeze );
                ofAddListener( blobs[blobid].unFreeze, this, &planeApp::blobUnFreeze );
                ofAddListener( blobs[blobid].overFreeze, this, &planeApp::blobOverFreeze );
                ofAddListener( blobs[blobid].prepareToDie, this, &planeApp::blobUnlink );
                ofAddListener( blobs[blobid].onCreate, this, &planeApp::blobOnCreate );
                ofAddListener( blobs[blobid].updatePosition, this, &planeApp::videoFollowBlob );
            }
            // update blob with new values
            Blob* b = &blobs.find(blobid)->second;
            b->follow(posx + blobW/2.0, posy + blobH/2.0, blobserverW, blobserverH, edgeMargin);
            b->setVelocity(velx, vely);
            b->analyze(freezeMaxVel, freezeMinTime, freezeMaxTime, movingThr);    //
            b->age = age;
            b->lostDuration = lost;
            if (newBlob) ofNotifyEvent( blobs[blobid].onCreate, blobid, &blobs[blobid] );
//			cout << "blob " << blobid << "   " << posx << "|" << posy << "   - lifetime: " << b->lifetime << "\n";
		}
		else{
			// unrecognized message
		}

	}}

//--------------------------------------------------------------
void planeApp::draw(){

    if (fullscreen) {

        ofBackground(0);
        ofPushMatrix();
        ofTranslate(0, 700);
        ofRotateZ(-90);

        this->drawScreen(0, 0, 1);

        string instruction = scenes[scene].instructions[segment];
        ofFill(); ofSetColor(255);
        font.drawString(instruction, 50, 50);

        ofPopMatrix();

        ofFill(); ofSetColor(255);
        ofDrawBitmapString(ofToString(ofGetFrameRate()), 10, 10);

    } else {

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
        offsx += 300;
        this->drawAnalysis(offsx, offsy, 0.35);

        offsy = 10;
        offsx += 390;
        this->drawScreen(offsx, offsy, 0.35);

        offsx += 400;
        this->drawControlInfo(offsx, offsy);

        gui.draw();

    }
}


//--------------------------------------------------------------
void planeApp::drawScreen(int x, int y, float scale){


    // black frame first
    ofFill(); ofSetColor(0);
    ofRect(x,y,projectionW*scale,projectionH*scale);
    ofNoFill(); ofSetColor(255);
    if (scale<1.0) {
        ofRect(x,y,projectionW*scale,projectionH*scale);
    }

    // background videos`
    for (vector<ofPtr<mediaElement> >::iterator it = bgVideos[scene].begin() ; it != bgVideos[scene].end(); ++it) {
        (**it).draw(x, y, scale);
    }

    // ofEnableAlphaBlending();
    ofEnableBlendMode(OF_BLENDMODE_ADD);

    // foreground videos
    for (vector<ofPtr<mediaElement> >::iterator it = fgMedia.begin(); it != fgMedia.end(); ++it) {
        (**it).draw(x, y, scale);   // if ((**it).blend) 
    }

    // extra things to draw? // will become video later on!
    if (scene==1 && !transition) {
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

    // foreground videos, without BLENDING mode
    ofEnableAlphaBlending();
    for (vector<ofPtr<mediaElement> >::iterator it = fgMedia.begin(); it != fgMedia.end(); ++it) {
        if (!(**it).blend) (**it).draw(x, y, scale);
    }
    ofDisableAlphaBlending();

    string instruction = scenes[scene].instructions[segment];
    ofFill(); ofSetColor(255);
    font.drawString(instruction, x+20, y+projectionH*scale*0.8);

    if (flash) {
        ofEnableAlphaBlending();
        int alpha = (flashCnt < flashMax/2) ? 255*(flashCnt/(flashMax/2.f)) : 255*(flashMax - flashCnt)/(flashMax/2.f);
        ofSetColor(flashColor.get().r,flashColor.get().g,flashColor.get().b,alpha); ofFill();
        ofRect(x,y,projectionW*scale,projectionH*scale);
        ofDisableAlphaBlending();
    }
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


                if(b->lostDuration > 0 && !b->onEdge) ofSetColor(255);  // b->lostDuration < hopLength 
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
                       "\nLENGTH\t\t" + ofToString(scenes[scene].length[segment]) +
                       "\nSUCCESS\t\t" + ofToString(success ? "true" : "false") +
                       "\nSUCCESS CNT\t" + ofToString(successCnt) +
                       // "\n\n" + instruction +
                       // "\n\nAUTOPLAY\t" + ofToString(autoplay ? "true" : "false") +
                       "\n\nTRANSITION\t" + ofToString(transition ? "true" : "false") +
                       "\nGLOBAL TIME\t" + ofToString(masterClock) +
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
        endSegment(-1);
    } else if (key == OF_KEY_RIGHT){
        endSegment(1);
    }
    if (key == ' ') {
        autoplay = !autoplay;
    }
    if(key == 's') {
		gui.saveToFile("planets01.xml");
	}
	if(key == 'l') {
		gui.loadFromFile("planets01.xml");
	}
    if (key=='f') {
        fullscreen = !fullscreen;
    }

    if (key=='p') {
        cout << "make screenshot" << endl;
        ofImage img;
        img.grabScreen(0,0,1425,700);
        string fileName = "plane_"+ofGetTimestampString()+".png";
        img.saveImage(fileName);
        cout << "saved screenshot " << fileName.c_str() << endl;
    }

    if (key>='0' && key<='5') {
        int s = int(key) - int('0');
        cout << "key input = " << key << " scene = " << s << endl;
        jumpToScene(s);
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
