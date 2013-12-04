#include "planeApp.h"
#include "ofBitmapFont.h"


std::ostream& tcout() {
  // Todo: get a timestamp in the desired format
  return std::cout << ofGetFrameNum() << " \t";
}

//--------------------------------------------------------------
void planeApp::setup(){

	ofTrueTypeFont::setGlobalDpi(72);

    fontBg.loadFont("Mercury Bold.otf", 36, true, true);
    fontBg.setLineHeight(30.0f);
    fontBg.setLetterSpacing(1.037);

    fontSm.loadFont("Mercury Bold.otf", 20, true, true);
	fontSm.setLineHeight(16.0f);
	fontSm.setLetterSpacing(1.037);

    language = 0;
    processing = true;
    oscMsgReceived = false;
    oscLastMsg = 0;
    oscLastMsgTimer = 0;
    oscLastMsgTimerMax = 10;
    oscActive = false;
	mouseX = 0;
	mouseY = 0;
	mouseButtonState = "";

    fullscreen = false;
    ofSetWindowPosition(0,0);
    success = false;
    successCnt = 0;
    blobsOnStage = 0;
    activityCnt = 0;
	drawBlobDetail = false;
    transition = false;
    moveOn = false;
    sceneChange = false;
    segmentChange = 1;
    flash = false;
    flashCnt = 0;
    flashMax = 20;

    drawBridge = false;
    bridgeX = 5;
    bridgeY = 55;

    projectionW = 1080;
    projectionH = 1920;
    blobserverW = 500;
    blobserverH = 300;
    blobW = 80;
    blobH = 160;

    hogFlowId = 1;
    bgsubtractorFlowId = 2; // 1 = hog, 2 = bgs, 3 = nop
    bgsubtractorCnt = 0;
    bgsubtractorVel = 0.f;
    bgsubtractorAvVel = 0.f;

    siteW.addListener(this,&planeApp::recalculatePerspective);
	siteH.addListener(this,&planeApp::recalculatePerspective);


	gui.setup("HUMANS AND PLANETS", "planets01.xml", 1104,190);
    gui.setDefaultBackgroundColor( ofColor(0,0,50) );
    gui.add(autoplay.setup("autoplay", false)); 
    gui.add(flashColor.set("Transition Flash Color",ofColor(255,200,100),ofColor(0,0),ofColor(255,255)));

    paramBasic.setName("Dimension"); 
    paramBasic.add(siteW.set( "Site Width", 500, 0, 1000));
    paramBasic.add(siteH.set( "Site Length", 500, 0, 1000));
    paramBasic.add(stageRadius.set( "Stage Radius", 200, 0, 500));
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
    paramSc1.add(newStarMax.set( "NewStar Max", 30, 1, 40));
    // paramSc1.add(newStarBonus.set( "Bonus every", 5, 1, 40));
    paramSc1.add(distStdDevThr.set( "Dist StdDev", 10, 0, 20));
    paramSc1.add(movingThr.set( "Movement Thr", 0.1, 0, 2));
    paramSc1.add(steadyRewardTime.set( "Dist Reward", 2, 0, 10));
    gui.add(paramSc1);

    paramSc2.add(spinJudgeTime.set( "spinJudgeTime", 180, 0, 240));
    paramSc2.add(spinSuccess.set( "spinSuccess", 5, 0, 10));
    paramSc2.add(spinFailure.set( "spinFailure", 2, 0, 10));
    paramSc2.setName("Sc2 Revolutions");
    gui.add(paramSc2);

    paramSc3.setName("Sc3 Sun");
    paramSc3.add(minLostTime.set( "Min LostTime", 1, 0, 10));
    paramSc3.add(edgeMargin.set( "Edge Margin", 50, 0, 150));
    gui.add(paramSc3);

    paramSc4.setName("Sc4 Alignment");
    paramSc4.add(alignmentMaxDist.set( "Alignm MaxDist", 50, 0, 200));
    paramSc4.add(alignmentTransition.set( "Alignm Transition", 10, 0, 20));
    gui.add(paramSc4);

    // paramSc5.setName("Sc5 Combustion");
    // gui.add(paramSc5);


	gui.setSize(200,500);
    gui.loadFromFile("planets01.xml");

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
    steles[0].set(383-78,184-16,0);
    steles[1].set(457-78,198-16,0);
    steles[2].set(506-78,224-16,0);
    steles[3].set(446-78,260-16,0);
    steles[4].set(312-78,276-16,0);
    steles[5].set(160-78,269-16,0);
    steles[6].set(159-78,231-16,0);
    steles[7].set(241-78,203-16,0);
    
    int tmp = 1;
    recalculatePerspective(tmp);

    // listen on the given port
	tcout() << "listening for osc messages on port " << PORT << "\n";
	receiver.setup(PORT);

	ofBackground(255);

}

void planeApp::recalculatePerspective(int & v) {
    tcout() << "recalculatePerspective" << endl;
    tcout() << "siteW " << siteW << "   siteH " << siteH << endl;
    this->setPerspective();

    for (int i=0; i<8; i++) {    
        vector<cv::Point2f> pre, post;
        pre.push_back(cv::Point2f(steles[i].x, steles[i].y));
        cv::perspectiveTransform(pre, post, perspectiveMat);
        steles_topdown[i].set(post[0].x, post[0].y,0);
    }
}

void planeApp::setPerspective() {
    int skew = 0;
    // 800x600  scale 0.4 >> 320x240   minus 64x128
    // float in[] = {skew,0,blobserverW-skew,0,blobserverW,blobserverH,0,blobserverH};
    // float in[] = {180,180,660,140,410,300,0,320};
    float in[] = {260-78,140-16,600-78,210-16,380-78,300-16,-150-78,300-16};
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


    nebula = ofPtr<mediaElement>( 
        new videoElement("video/NEBULA-H264-10mbps.mp4"));
    nebula->reset(true);
    bgMedia = ofPtr<mediaElement>( 
        // new videoElement("video/BACKGROUND-1_loop-H264_10mpbs.mp4"));
        new videoElement("video/IDLE_MODE_11-half-1-H264-10mbps.mp4"));
    // bgMedia->reset(true);
    // bgMedia->outroTransformation = &mediaElement::finishFast; 
    // ofAddListener( bgMedia->fadeOutEnd, this, &planeApp::bgMediaSwap );


    sceneInfo idle;
    idle.name = "Idle";
    idle.no = n;
    idle.segments = 1;
    idle.analysis[0] = "* detection";
    idle.instructions[0][0] = "Come closer";
    idle.instructions[1][0] = "Approchez";
    idle.length[0] = -1;
    scenes[n] = idle;


    n++;

    sceneInfo stars;
    stars.name = "Star Links";
    stars.no = n;
    stars.segments = 4;
    stars.instructions[0][0] = "Stand still";
    stars.instructions[1][0] = "Ne bougez plus";
    stars.analysis[0] = "* Velocity < FreezeMaxVel\n* frozenTimer > freezeMinTime\n-> 20 sec";
    stars.length[0] = 20;
    stars.instructions[0][1] = "Try new spots to light up more stars";
    stars.instructions[1][1] = "Essayez de nouveaux \nendroits pour allumer \nplus d'étoiles";
    stars.analysis[1] = "* Velocity < FreezeMaxVel\n* frozenTimer > freezeMinTime\n* frozenTimer < freezeMaxTime\n+ Star Animation at end of freezeMaxTime\n-> newStarMax stars || 40 sec";
    stars.length[1] = 40;
    stars.instructions[0][2] = "Walk with someone. \nKeep the same distance between you.\n(No hands!)";
    stars.instructions[1][2] = "Marchez avec un partenaire\nGardez la même distance entre vous\n(Sans les mains!)";
    stars.analysis[2] = "* velocity history > movingThr \n* distance history < distStdDevThr\n-> 45 sec";
    stars.length[2] = 45;
    stars.instructions[0][3] = "Walk with \nsomeone. \nMake eye \ncontact. \nKeep the \ndistance.";
    stars.instructions[1][3] = "Marchez avec un partenaire \nGardez la même distance entre vous \nRegardez-vous dans les yeux";
    stars.analysis[3] = "* velocity history > movingThr \n* distance history < distStdDevThr\n-> 60 sec";
    stars.length[3] = 60;
    scenes[n] = stars;

    n++;

    sceneInfo revolution;
    revolution.name = "Revolution";
    revolution.no = n;
    revolution.segments = 2;
    revolution.instructions[0][0] = "Take someone's hand \nSpin and lean out as far as possible";
    revolution.instructions[1][0] = "Tenez les mains de votre partenaire \nTournez";
    revolution.analysis[0] = "* \n-> 20 sec";
    revolution.length[0] = 20;
    revolution.instructions[0][1] = "Let go...";
    revolution.instructions[1][1] = "Lâchez !";
    revolution.analysis[1] = "* \n-> 20 sec";
    revolution.length[1] = 20;
    scenes[n] = revolution;

    n++;

    sceneInfo sun;
    sun.name = "Explosion";
    sun.no = n;
    sun.segments = 6;
    sun.instructions[0][0] = "Stand on one leg.";
    sun.instructions[1][0] = "Tenez-vous sur une jambe";
    sun.analysis[0] = "- \n-> 25 sec";
    sun.length[0] = 25;
    sun.instructions[0][1] = "Hop from one \nspot to the other.";
    sun.instructions[1][1] = "Déplacez-vous en sautant";
    sun.analysis[1] = "* onLost event\n-> 20 sec";
    sun.length[1] = 20;
    sun.instructions[0][2] = "Everyone in unison";
    sun.instructions[1][2] = "Tous en même temps";
    sun.analysis[2] = "* onLost event\n-> 20 sec";
    sun.length[2] = 20;
    sun.instructions[0][3] = "FREEZE!";
    sun.instructions[1][3] = "STOP!";
    sun.analysis[3] = "* velocity < freezeMaxVel\n-> 20 sec || all frozen";
    sun.length[3] = 20;
    sun.instructions[0][4] = "Run in every \ndirection at once.";
    sun.instructions[1][4] = "Courez partout en même temps";
    sun.analysis[4] = "* \n-> 40 sec";
    sun.length[4] = 40;
    sun.instructions[0][5] = "FREEZE!";
    sun.instructions[1][5] = "STOP!";
    sun.analysis[5] = "* velocity < freezeMaxVel\n-> 20 sec || all frozen";
    sun.length[5] = 13;
    scenes[n] = sun;

    n++;

    sceneInfo eclipse;
    eclipse.name = "Alignment";
    eclipse.no = n;
    eclipse.segments = 7;
    eclipse.instructions[0][0] = "Now line-up in front of me";
    eclipse.instructions[1][0] = "Faites une file face à moi.";
    eclipse.analysis[0] = "- \n-> 20 sec";
    eclipse.length[0] = 20;
    eclipse.instructions[0][1] = "Follow me";
    eclipse.instructions[1][1] = "Suivez-moi.";
    eclipse.analysis[1] = "\n-> 20 sec";
    eclipse.length[1] = 20;
    eclipse.instructions[0][2] = "Step out of the line";
    eclipse.instructions[1][2] = "Sortez de la file.";
    eclipse.analysis[2] = "\n-> 10 sec";
    eclipse.length[2] = 10;
    eclipse.instructions[0][3] = "Step into the line";
    eclipse.instructions[1][3] = "Revenez dans la file.";
    eclipse.analysis[3] = "* x == main.x\n-> 10 sec";
    eclipse.length[3] = 10;
    eclipse.instructions[0][4] = "Step out of the line";
    eclipse.instructions[1][4] = "Sortez de la file.";
    eclipse.analysis[4] = "\n-> 10 sec";
    eclipse.length[4] = 10;
    eclipse.instructions[0][5] = "Step into the line";
    eclipse.instructions[1][5] = "Revenez dans la file.";
    eclipse.analysis[5] = "* x == main.x\n-> 10 sec";
    eclipse.length[5] = 10;
    eclipse.instructions[0][6] = "Disperse very slowly towards the edges.";
    eclipse.instructions[1][6] = "Dispersez-vous lentement vers l'extérieur.";
    eclipse.analysis[6] = "* edge-proximity = opacity \n-> 15 sec";
    eclipse.length[6] = 15;
    scenes[n] = eclipse;

    n++;

    sceneInfo shooting;
    shooting.name = "Combustion";
    shooting.no = n;
    shooting.segments = 4;
    shooting.instructions[0][0] = "Move like a \nshooting star.";
    shooting.instructions[1][0] = "Bougez comme une étoile filante";
    shooting.analysis[0] = "* onLost event \n-> 25 sec";
    shooting.length[0] = 25;
    shooting.instructions[0][1] = "Drop to the \nground!";
    shooting.instructions[1][1] = "Laissez-vous tomber";
    shooting.analysis[1] = "* onLost event \n-> 15 sec";
    shooting.length[1] = 15;
    shooting.instructions[0][2] = "Exhale.\nLook at the sky.";
    shooting.instructions[1][2] = "Regardez le ciel\nExpirez";
    shooting.analysis[2] = "- \n-> 15 sec";
    shooting.length[2] = 15;
    shooting.instructions[0][3] = "Stand up";
    shooting.instructions[1][3] = "Relevez-vous.";
    shooting.analysis[3] = "- \n-> 10 sec";
    shooting.length[3] = 10;
    shooting.instructions[0][4] = "Thank you";
    shooting.instructions[1][4] = "Merci";
    shooting.analysis[4] = "- \n-> 5 sec";
    shooting.length[4] = 5;
    scenes[n] = shooting;

    tcout() << "there are " << scenes.size() << " scenes" << endl;

    sceneChange = true;
    nextSegment(1);

}

//--------------------------------------------------------------
void planeApp::update(){

    if (processing) {

        oscMsgReceived = false;
        this->receiveOsc();

        if (!oscMsgReceived) {
            oscLastMsgTimer = ofGetUnixTime() - oscLastMsg;
            if (oscActive && oscLastMsgTimer > oscLastMsgTimerMax) {
                oscActive = false;
            }
            // tcout() << "." << endl;
        }

        if (oscMsgReceived) {

        	// BLOB CLEANUP
            std::map<int,Blob>::iterator it = blobs.begin();
            while (it != blobs.end()) {
                Blob* b = &it->second;
            	b->update(minLostTime);
            	if( !b->isAlive() ) {
                    // tcout() << "Blob dead\t" << b->id << endl;
                    ofNotifyEvent(b->prepareToDie,b->id,b);
            		blobs.erase(it++);
            	} else {
            		++it;
            	}
            }
            // tcout() << "Blob count\t" << blobs.size() << "\t\tvideo count\t" << fgMedia.size() << endl;

            // ANALYSIS
            std::map<int, ofPoint> blobPositions;
            for(std::map<int, Blob>::iterator it = blobs.begin(); it != blobs.end(); ++it){
                Blob* b = &it->second;
                blobPositions[b->id] = b->position;
            }
            for(std::map<int, Blob>::iterator it = blobs.begin(); it != blobs.end(); ++it){
                Blob* b = &it->second;
                b->analyze(freezeMaxVel, freezeMinTime, freezeMaxTime, movingThr);
            }
            for(std::map<int, Blob>::iterator it = blobs.begin(); it != blobs.end(); ++it){
                Blob* b = &it->second;
                b->analyzeNeighbors(blobPositions, distStdDevThr, steadyRewardTime);
            }

            blobsOnStage = 0;
            for(std::map<int, Blob>::iterator it = blobs.begin(); it != blobs.end(); ++it){
                Blob* b = &it->second;
                if (b->onStage) blobsOnStage++;
            }
        }

        // SCHEDULING
        masterClock = ofGetUnixTime() - globalStart;
        segmentClock = ofGetUnixTime() - segmentStart;
        if (flash) {
            flashCnt++;
            if (flashCnt > flashMax) {
                // tcout() << "flash done" << endl;
                flashCnt = 0;
                flash = false;
                beginSegment();
            }
        }
        // TIME TRIGGERS
        if (autoplay && scenes[scene].length[segment] > 0 && segmentClock >= scenes[scene].length[segment] && !transition) {
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
            if(autoplay && success && !transition) {
                tcout() << "scene 0 success" << endl;
                endSegment(1);
            }
        } else if (scene==1) {
            if (autoplay && segment==1 && successCnt > newStarMax && !transition) {
                tcout() << "scene 1 segment 1 success" << endl;
                endSegment(1);
            }
        } else if (scene==3) {
            if (segment==4 || segment==6 ) {
                // FREEZE!
                if(autoplay && success && !transition) {
                    tcout() << "FREEZE success" << endl;
                    endSegment(1);
                }
            }
        }

        nebula->update();
        bgMedia->update();

        vector<ofPtr<mediaElement> >::iterator iter = fgMedia.begin();
        while (iter != fgMedia.end()) {
            (**iter).update();
            if ((**iter).dead) {
                tcout() << "delete video " << (**iter).file << endl;
                iter = fgMedia.erase(iter);
            } else {
                ++iter;
            }
        }

        // animation related
        if (scene==2) {
            if (!transition && ofGetFrameNum()%spinJudgeTime==0) {
                if (segment==0) {
                    int oldCnt = planetCnt;
                    // set planetCnt based on activityCnt
                    if (activityCnt >= spinSuccess) planetCnt++;
                    else if (activityCnt < spinFailure) planetCnt--;
                    if (planetCnt>5) planetCnt = 5;
                    else if (planetCnt<0) planetCnt = 0;
                    if (oldCnt!=planetCnt) positionRevolutions();
                } else if (segment==1) {
                    int oldCnt = planetCnt;
                    if (activityCnt < spinFailure) planetCnt--;
                    if (planetCnt<oldCnt && planetCnt>=0) {
                        int videoPick = planetCnt+1;
                        (*fgMedia[planetCnt]).loadMovie("video/3_revolution/REV_0"+ofToString(videoPick)+"-out-animation.mov");
                        (*fgMedia[planetCnt]).reset(true);
                        (*fgMedia[planetCnt]).autoDestroy(true);
                        if (planetCnt<=0) success = true;
                    }
                    // if (fgMedia.size()>planetCnt) {
                    // }
                }
                tcout() << "\tactivityCnt\t" << activityCnt << "\tplanetCnt\t" << planetCnt << endl;
                activityCnt = 0;
            }
        } else if (scene==3 && segment==5) {
            if (ofRandom(100)<2) (*fgMedia[0]).bounce();
        } else if (!transition && segmentClock > alignmentTransition && scene==4 && blobs.size()>0) {
            // check if all are aligned
            bool allAligned = allBlobsAlignedWith((*fgMedia[0]).position);

            if (allAligned) {
                if (!success) {
                    // aligned
                    success = true;
                    string alignmentGlow = "video/5_eclipse/ECLIPSE_alignment-glow-01-animation.mov";
                    fgMedia.push_back(ofPtr<mediaElement>( new videoElement(alignmentGlow) ));
                    (*fgMedia[fgMedia.size()-1]).setDisplay(projectionW/2, projectionH/2, true);
                    (*fgMedia[fgMedia.size()-1]).reset(true);
                    (*fgMedia[fgMedia.size()-1]).autoDestroy(true);
                }
            } else {
                success = false;
            }
        }

    }

}

void planeApp::bgMediaSwap(int & trans) {
    tcout() << "bgMediaSwap\t\tscene: " << scene << "   bgMedia->id: " << bgMedia->id << endl;

    if (scene==0 && (bgMedia->id==-1 || bgMedia->id==2)) {
        tcout() << "\t\t\tidle" << endl;
        bgMedia->loadMovie("video/IDLE_MODE_11-half-1-H264-10mbps.mp4");
        bgMedia->id = 0;
        bgMedia->reset(true);   // play right away
        bgMedia->movieEndTrigger=false;
        bgMedia->outroTransformation = &mediaElement::finishMovie; 
        ofAddListener( bgMedia->fadeOutEnd, this, &planeApp::bgMediaSwap );
    } else if (scene==0 && bgMedia->id==0) {
        tcout() << "\t\t\tstarry intro" << endl;
        bgMedia->loadMovie("video/BACKGROUND-1_intro-H264_10mpbs.mp4");
        bgMedia->id = 1;
        bgMedia->reset(true);
        bgMedia->finishMovie(1.0);
        bgMedia->movieEndTrigger=true;
        ofAddListener( bgMedia->fadeOutEnd, this, &planeApp::bgMediaSwap );
    } else {
        nextSegment(1);
        tcout() << "\t\t\tstarry bg" << endl;
        bgMedia->loadMovie("video/BACKGROUND-1_loop-H264_10mpbs.mp4");
        bgMedia->id = 2;
        bgMedia->reset(true);
        bgMedia->movieEndTrigger=false;
        bgMedia->autoDestroy(false);
        ofRemoveListener( bgMedia->fadeOutEnd, this, &planeApp::bgMediaSwap );
    }

}

void planeApp::bgMediaFadedOut(int & trans) {
    tcout() << "bgMediaFadedOut" << endl;
    // set moveOn to true, instead of calling nextSegment()
    // to avoid conflicting threading
    moveOn = true;
}

// trans -1 : trigger first fade from main-loop and not from faded-event
void planeApp::fgMediaFadedOut(int & trans) {
    tcout() << "fgMediaFadedOut  " << trans << "  fgmedia size = " << fgMedia.size() << endl;
    
    if (fgMedia.size()>0) {
        
        if (trans == -1) {
            // first call of function, therefore we fade the last fgMedia object
            ofAddListener( (*fgMedia.back()).fadeOutEnd, this, &planeApp::fgMediaFadedOut );
            tcout() << "call outroTransformation() on fgMedia[" << (fgMedia.size()-1) << "]" << endl;
            ((*fgMedia.back()).*((*fgMedia.back()).outroTransformation))();

        } else {
            // delete last fgMedia object
            (*fgMedia.back()).dead = true;

            if (fgMedia.size()>1) {
                // fade new last fgMedia object
                ofAddListener( (*fgMedia[fgMedia.size()-2]).fadeOutEnd, this, &planeApp::fgMediaFadedOut );
                tcout() << "call outroTransformation() on fgMedia[" << (fgMedia.size()-2) << "]" << endl;
                ((*fgMedia[fgMedia.size()-2]).*((*fgMedia[fgMedia.size()-2]).outroTransformation))();
            } else {
                // fade BG now
                if (scene==0) ((*bgMedia).*((*bgMedia).outroTransformation))();
                else moveOn = true;
            }
        }
    } else {
        // fade BG now
        if (scene==0) ((*bgMedia).*((*bgMedia).outroTransformation))();
        else moveOn = true;
    }
}

void planeApp::fgMediaFadedIn(int & trans) {
    tcout() << "fgMediaFadedIn  " << trans << endl;
    if (fgMedia.size() > 0) {
        for (vector<ofPtr<mediaElement> >::iterator it = fgMedia.begin(); it != fgMedia.end(); it++) {
            if (!(**it).visible) {
                tcout() << "fgMediaFadedIn   " << (**it).file << endl;
                ofAddListener( (**it).fadeInEnd, this, &planeApp::fgMediaFadedIn );
                ((**it).*((**it).introTransformation))();
            }
        }
    }
}

void planeApp::bgMediaFadedIn(int & trans) {
    tcout() << "bgMediaFadedIn" << endl;
    transition = false;

    // make new blob connections, to ensure blobs are connected to video elements, if necessary
    for(std::map<int, Blob>::iterator it = blobs.begin(); it != blobs.end(); ++it){
        // tcout() << "new blob connection " << endl;
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
    // tcout() << "BLOB " << blobID << " just got lost" << endl;
    if (!transition && blobs[blobID].onStage) {
        if (scene==2) {
            activityCnt++;
        } else if (scene==3) {
            // SUN explosions
            if (segment==1 || segment==2) {
                int randomExpl = ofRandom(6) + 1;
                // string newVideoName = "video/4_sun/SUN_explosion-" + ofToString(randomExpl,2,'0') + "-animation.mov";
                string newVideoName = "video/4_sun/SUN_explosion-" + ofToString(randomExpl,2,'0') + "-animation.mp4";
                // newVideoName = "video/4_sun/SUN_explosion-01-H264-10mbps.mp4";
                // newVideoName = "video/4_sun/SUN_explosion-01-photoJPEG.mov";
                // newVideoName = "video/4_sun/SUN_explosion-01-animation_alpha.mov";
                newVideoName = "video/4_sun/SUN_explosion-01-qtTGA.mov";
                fgMedia.push_back(ofPtr<mediaElement>( new videoElement(newVideoName,false)));
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
                (*fgMedia[fgMedia.size()-1]).moveAcross( blobs[blobID].velocity.x, blobs[blobID].velocity.y, projectionW, projectionH, true);
                (*fgMedia[fgMedia.size()-1]).reset();
            } else if (segment==1) {
                float randdeg = ofRandom(-5.f, 5.f);
                for (int i=0; i<10; i++) {
                    int randomShooter = ofRandom(6) + 1;
                    fgMedia.push_back(ofPtr<mediaElement>( new videoElement("video/6_shooting/SSTAR_0" + ofToString(randomShooter) + "-H264-10mbps.mp4")));
                    (*fgMedia[fgMedia.size()-1]).setDisplay(ofRandom(projectionW-100), ofRandom(projectionH-100), true);
                    (*fgMedia[fgMedia.size()-1]).moveAcross( randdeg, 45.f, projectionW, true);
                    (*fgMedia[fgMedia.size()-1]).reset();
                    // (*fgMedia[fgMedia.size()-1]).movie->setFrame(i);
                }
            }
        }
    }
}

void planeApp::blobSteady(Pair & pair) {
    // add particle trail video between stars
    if (!transition && scene==1 && segment>1) {
        tcout() << "blobSteady() \t\t" << pair.blob1 << " + " << pair.blob2 << endl;

        Blob* b1 = &blobs[pair.blob1];
        Blob* b2 = &blobs[pair.blob2];

        // check if bridge already exists
        bool exists = false;
        for (vector<ofPtr<mediaElement> >::iterator it = fgMedia.begin(); it != fgMedia.end(); it++) {
            if ((**it).bridgeVideo && (**it).bridgeBlobID[0]==pair.blob1 && (**it).bridgeBlobID[1]==pair.blob2) {
                exists = true;
                break;
            }
        }

        if (!exists) {
            tcout() << "\t\t\tadd video bridge" << endl;
            string newVideoName = "video/2_stars/ATTRACTION_link-01-animation.mov";
            fgMedia.push_back(ofPtr<mediaElement>( new videoElement(newVideoName)));
            (*fgMedia[fgMedia.size()-1]).reset();
            (*fgMedia[fgMedia.size()-1]).bridge(b1->id, b2->id);
            ofNotifyEvent( blobs[pair.blob1].updatePosition, pair.blob1, &blobs[pair.blob1] );
        } else {
            tcout() << "\t\t\talready exists" << endl;
        }
    }
}

void planeApp::blobSteadyReward(Pair & pair) {
    // tcout() << "blobSteadyReward() \t" << pair.blob1 << " + " << pair.blob2 << endl;
    if (!transition && scene==1 && segment>1) {
        ofPtr<mediaElement> vid1 = blobs[pair.blob1].mediaLink;
        ofPtr<mediaElement> vid2 = blobs[pair.blob2].mediaLink;

        // replace video with sparklier star
        if (vid1 != NULL) {
            // tcout() << "blob " << pair.blob1 << " \t\tfound vid" << endl;
            (*vid1).loadMovie("video/2_stars/ATTRACTION_star_glow-01-animation.mov");
            (*vid1).reset();
        }
        if (vid2 != NULL) {
            // tcout() << "blob " << pair.blob2 << " \t\tfound vid" << endl;
            (*vid2).loadMovie("video/2_stars/ATTRACTION_star_glow-01-animation.mov");
            (*vid2).reset();
        }
    }
}

void planeApp::blobBreakSteady(Pair & pair) {
    if (!transition && scene==1 && segment>1) {
        tcout() << "blobBreakSteady() \t" << pair.blob1 << " + " << pair.blob2 << endl;

        ofPtr<mediaElement> vid1 = blobs[pair.blob1].mediaLink;
        ofPtr<mediaElement> vid2 = blobs[pair.blob2].mediaLink;

        // replace video with normal star
        if (vid1 != NULL) {
            // tcout() << "blob " << pair.blob1 << " \t\tfound vid" << endl;
            (*vid1).loadMovie("video/2_stars/INDIV STAR 2 loop-H264-10mbps.mp4");
            (*vid1).reset();
        }
        if (vid2 != NULL) {
            // tcout() << "blob " << pair.blob2 << " \t\tfound vid" << endl;
            (*vid2).loadMovie("video/2_stars/INDIV STAR 2 loop-H264-10mbps.mp4");
            (*vid2).reset();
        }
        // delete bridge video if it exists
        bool found = false;
        for (vector<ofPtr<mediaElement> >::iterator it = fgMedia.begin(); it != fgMedia.end(); it++) {
            if ((**it).bridgeVideo && (**it).bridgeBlobID[0]==pair.blob1 && (**it).bridgeBlobID[1]==pair.blob2) {
                tcout() << "\t\t\tunlinked bridge" << endl;
                (**it).dead = true;
                found = true;
            }
        }
        if (!found) tcout() << "\t\t\tbridge not found" << endl;

    }
}




void planeApp::blobOnFreeze(int & blobID) {
//    tcout() << "BLOB " << blobID << " froze!" << endl;
    if (!transition) {
        if (scene==1) {
            if ((segment==0 || segment==1) && blobs[blobID].onStage) {
                // STAND STILL
                // create new video element
    //            tcout() << "blob " << blobID << " \t\tfreeze ! " << endl;
                fgMedia.push_back(ofPtr<mediaElement>( new videoElement("video/2_stars/INDIV STAR 2 loop-H264-10mbps.mp4")));
                blobs[blobID].mediaLink = fgMedia[fgMedia.size()-1];
                // (*fgMedia[fgMedia.size()-1]).setDisplay(ofRandom(projectionW-100), ofRandom(projectionH-100));
                (*fgMedia[fgMedia.size()-1]).setDisplay(offsetX + blobs[blobID].position.x * mapSiteW, offsetY + blobs[blobID].position.y * mapSiteH);
                (*fgMedia[fgMedia.size()-1]).reset();
                (*fgMedia[fgMedia.size()-1]).outroTransformation = &mediaElement::scaleAway;
                if (segment==1) successCnt++;
            }
        } else if (scene==3) {
            if ((segment==4 || segment==6) && !success) {
                // FREEZE!
                // check if all blobs are frozen
                // frozen
                bool allFrozen = true;
                for(std::map<int, Blob>::iterator it = blobs.begin(); it != blobs.end(); ++it){
                    Blob* b = &it->second;
                    if (!b->frozen && b->onStage) allFrozen = false;
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
    //            tcout() << "BLOB " << blobID << " moved again!  mediaLink = " << blobs[blobID].mediaLink << endl;
                // STAND STILL
                tcout() << "blobUnFreeze()\t\t" << blobID << endl;
                // video unlink
                // video fade away
                ofPtr<mediaElement> vid = blobs[blobID].mediaLink; 
                if (vid != NULL) {
                    vid->movie->setPaused(true);
                    vid->fadeOut(0.001, 0.5, true);
                    blobs[blobID].mediaLink = ofPtr<mediaElement>();
                }
            }
        }
    }
}

void planeApp::blobOverFreeze(int & blobID) {
    if (!transition) {
        if (scene==1) {
            if (segment==1) {
                tcout() << "blobOverFreeze()\t\t" << blobID << endl;
                // rewarding star animation video, placed at star position
                string bonusVideo = "video/2_stars/HAND animation-QTAnimation.mov";
                fgMedia.push_back(ofPtr<mediaElement>( new videoElement(bonusVideo) ));
                float x = offsetX + blobs[blobID].position.x * mapSiteW;
                float y = offsetY + blobs[blobID].position.y * mapSiteH;
                (*fgMedia[fgMedia.size()-1]).setDisplay(x, y);
                (*fgMedia[fgMedia.size()-1]).reset(true);
                (*fgMedia[fgMedia.size()-1]).autoDestroy(true);

                this->blobUnlink(blobID);
            }
        }
    }
}

void planeApp::blobEnterStage(int & blobID) {
    if (!transition) {
        // then assign the appropriate ones
        if (scene==0) {
            // COME CLOSER, recognition of people == time to move on
            if (!success) success = true;

        } else if (scene==1) {
            if (segment==2 || segment==3) {
                // KEEP THE DISTANCE
                // create new video element, but only show if inside the stage? 
    //            tcout() << "blob " << blobID << " \t\tfreeze ! " << endl;
                fgMedia.push_back(ofPtr<mediaElement>( new videoElement("video/2_stars/INDIV STAR 2 loop-H264-10mbps.mp4")));
                blobs[blobID].mediaLink = fgMedia[fgMedia.size()-1];
                blobs[blobID].videoTrace = true;
                (*fgMedia[fgMedia.size()-1]).setDisplay(0, 0, true);
                // (*fgMedia[fgMedia.size()-1]).setDisplay(ofRandom(projectionW-100), ofRandom(projectionH-100));
                (*fgMedia[fgMedia.size()-1]).reset();
                (*fgMedia[fgMedia.size()-1]).fadeIn();
                (*fgMedia[fgMedia.size()-1]).outroTransformation = &mediaElement::scaleAway;
            }
        } else if (scene==4) {
            // PLANETS
            int randomPlanet = ofRandom(23) + 1;
            // int randomPlanet = ofRandom(3) + 18;
            fgMedia.push_back(ofPtr<mediaElement>( new imageElement("video/5_eclipse/PLANET_" + ofToString(randomPlanet)+".png", 0.5f)));
            // fgMedia.push_back(ofPtr<mediaElement>( new videoElement("video/5_eclipse/PLANET_"+ofToString(randomPlanet)+".mov")));
            blobs[blobID].mediaLink = fgMedia[fgMedia.size()-1];
            blobs[blobID].videoTrace = true;
            float x = offsetX + blobs[blobID].position.x * mapSiteW;
            float y = offsetY + blobs[blobID].position.y *mapSiteH;
            (*fgMedia[fgMedia.size()-1]).setDisplay(x, y, true);
            (*fgMedia[fgMedia.size()-1]).reset();
            // (*fgMedia[fgMedia.size()-1]).introTransformation = &mediaElement::moveInFromSide;
            // tcout() << "init planet   pos: " << x << "/" << y << endl;
            if (segment==0 && segmentClock < alignmentTransition) {
                (*fgMedia[fgMedia.size()-1]).moveInFromSide(projectionW/2,projectionH/2);
            } else {
                (*fgMedia[fgMedia.size()-1]).fadeIn();
            }
        }
    }
}

void planeApp::blobLeaveStage(int & blobID) {
    if (!transition) {
        if (scene==1) {
            if (segment==2 || segment==3) {
                tcout() << "blobLeaveStage()\t\t" << blobID << endl;
                this->blobUnlink(blobID);
            }
        } else if (scene==4) {
            tcout() << "blobLeaveStage()\t\t" << blobID << endl;
            this->blobUnlink(blobID);
        }
    }
}

void planeApp::blobOnCreate(int & blobID) {
   
    if (!transition) {
        // first clean up, all related blob settings
        tcout() << "blobOnCreate() \t" << blobID << endl;
        this->blobUnlink(blobID);
        blobs[blobID].videoTrace = false;
        blobs[blobID].mediaLink = ofPtr<mediaElement>();  // TODO how to release ofPtr ? 
        
        if (blobs[blobID].onStage) {
            this->blobEnterStage(blobID);
        }
    }
}

// 
void planeApp::positionRevolutions() {

    if (fgMedia.size()>=5 ) {
        // REVOLUTION

        // there should only be 5 fgMedia elements
        for (unsigned int i=0; i<fgMedia.size(); i++) {
            (*fgMedia[i]).hide = true;
            if (i < planetCnt) (*fgMedia[i]).hide = false;
            (*fgMedia[i]).rotation = 0;
        }

        // positioning
        switch (planetCnt) {
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
    // tcout() << "blob " << blobID << " \t\tvideoFollowBlob" << endl;
    // find videoElement
    if (!transition) {
        ofPtr<mediaElement> vid = blobs[blobID].mediaLink; 
        // update position
        float bx;
        float by;
        if (vid != NULL) {
            // tcout() << "blob " << blobID << " \t\tfound vid" << endl;

            if (scene==4 && (segment==2 || segment==3)) {
                // PLANETS aligned on horizontal, move left right
                bx = offsetX + blobs[blobID].position.x * mapSiteW;
                by = offsetY + projectionH/2.0;
            } else if (scene==4 && (segment==4 || segment==5)) {
                // PLANETS aligned on vertical, move up down
                by = offsetX + blobs[blobID].position.x * mapSiteW * (16.0/9.0);
                bx = offsetX + projectionW/2.0;
            } else {
                bx = offsetX + blobs[blobID].position.x * mapSiteW;
                by = offsetY + blobs[blobID].position.y *mapSiteH;
            }

            if (!(*vid).moveElement) {
                (*vid).setDisplay( bx, by, true);
            } else {
                (*vid).goal.set( bx, by );
            }
        }
        // update position of sparkly bridge
        if (scene==1 && segment>1) {
            // check if bridge exists
            ofPtr<mediaElement> bridge; 
            int blob2ID;
            for (vector<ofPtr<mediaElement> >::iterator it = fgMedia.begin(); it != fgMedia.end(); it++) {
                if ((**it).bridgeVideo && !(**it).bridgeUpdated && ((**it).bridgeBlobID[0]==blobID || (**it).bridgeBlobID[1]==blobID)) {
                    bridge = (*it);
                    blob2ID = (**it).bridgeBlobID[0]==blobID ? (**it).bridgeBlobID[1] : (**it).bridgeBlobID[0];
                    break;
                }
            }

            if (bridge != NULL) {
                float bx2 = offsetX + blobs[blob2ID].position.x * mapSiteW;
                float by2 = offsetY + blobs[blob2ID].position.y *mapSiteH;
                float rot = -ofRadToDeg(atan2(bx2-bx,by2-by)) + 90;
                float tx = bridgeX;
                float ty = bridgeY;
                float tz = sqrt( pow(tx,2) + pow(ty,2) );
                float addRot = ofRadToDeg(atan2(tx,-ty));
                float cx = bx - tz*sin(ofDegToRad(rot+addRot));
                float cy = by + tz*cos(ofDegToRad(rot+addRot));

                (*bridge).setDisplay( cx, cy, false);
                (*bridge).bridgeUpdated = true;;
                (*bridge).w = ofDist(bx, by, bx2, by2) + tx*2;
                // (*bridge).h = ty*2;
                (*bridge).rotation = rot;
                // tcout() << "bridge udpate\t" << blobID << ":" << blob2ID << "\t" << bx << "|" << by << "  rot: " << rot << endl;
            }
        }
    }
}

void planeApp::blobUnlink(int & blobID) {
    // making sure, a blob goes to die and untethers all connections
    tcout() << "blobUnlink() \t" << blobID << "\tmediaLink: " << blobs[blobID].mediaLink << endl;
    for (vector<ofPtr<mediaElement> >::iterator it = fgMedia.begin(); it != fgMedia.end(); it++) {
        if (*it == blobs[blobID].mediaLink) {
            blobs[blobID].mediaLink = ofPtr<mediaElement>();
            tcout() << "\t\t\tunlinked blob " <<  endl;
            (**it).dead = true;
            break;
        }
    }
    // delete bridge video if it exists
    for (vector<ofPtr<mediaElement> >::iterator it = fgMedia.begin(); it != fgMedia.end(); it++) {
        if ((**it).bridgeVideo && ((**it).bridgeBlobID[0]==blobID || (**it).bridgeBlobID[1]==blobID)) {
            tcout() << "\t\t\tunlinked bridge\t" << (**it).bridgeBlobID[0] << " " << (**it).bridgeBlobID[1] << endl;
            (**it).dead = true;
            break;
        }
    }
}

void planeApp::beginSegment() {
    tcout() << "beginSegment()" << endl;
    int tmp = 1;
    bgMediaFadedIn(tmp);
}

void planeApp::endSegment(int direction) {
    tcout() << "endSegment()   " << scene << ":" << segment << endl;
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
    if (scene==0) bgMediaSwap(scene);
}

void planeApp::nextSegment(int direction) {
    segment+=direction;

    if(scene == -1 || segment >= scenes[scene].segments) {
        scene++;
        segment = 0;
        if(scene >= scenes.size()) {
            language = (language==0) ? 1 : 0;
            if (blobsOnStage==0) scene = 0;
            else scene = scenes.size()-1;
        }
        if (scene==0) {
            bgMediaSwap(scene);
            globalStart = ofGetUnixTime();
        }
    } else if (segment < 0){
        scene--;
        if(scene < 0){
            scene = scenes.size()-1;
            language = (language==0) ? 1 : 0;
        }
        segment = scenes[scene].segments -1;
    }

    if (sceneChange) tcout() << "scene " << scene << ": " << scenes[scene].name << endl;
    tcout() << "------------------------------------------\t" << scene << " : " << segment << endl;

    initSegment();
}

void planeApp::initSegment(){

    fgMedia.clear();   // delete all foreground videos
    success = false;
    successCnt = 0;
    activityCnt = 0;
    if (segment==0) planetCnt = 0;
    flash = true;       // 
    segmentStart = ofGetUnixTime();

    // add FG videos
    if (scene==2) {
        // REVOLUTIONS
        for (int i=0; i<5; i++) {
            string videoFile;
            int videoPick = i+1;
            videoFile = "video/3_revolution/REV_0"+ofToString(videoPick)+"-animation.mov";
            fgMedia.push_back(ofPtr<mediaElement>( new videoElement(videoFile,false)));
            if (segment==1 && i>=planetCnt) (*fgMedia[i]).dead = true;
            if (sceneChange) (*fgMedia[i]).reset(false);
            else (*fgMedia[i]).reset();
        }

        positionRevolutions();  // to position and turn on/off videos
    } else if (scene==3) {
        // SUN
        // fgMedia.push_back(ofPtr<mediaElement>( new videoElement("video/4_sun/SUN_8-ASSET-animation.mov")));
        fgMedia.push_back(ofPtr<mediaElement>( new videoElement("video/4_sun/SUN_15-qtPNG.mov",false)));
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
        if (segment>6) {
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
        oscMsgReceived = true;
        if (!oscActive) oscActive =true;
        oscLastMsg = ofGetUnixTime();

		// get the next message
		ofxOscMessage m;
		receiver.getNextMessage(&m);

		if(m.getAddress() == "/blobserver/startFrame"){

            currentFlowId = m.getArgAsInt32(1);
            // tcout() << "start frame flow " << flowid << endl;
            if (currentFlowId==bgsubtractorFlowId) {
                bgsubtractorCnt = 0;
                bgsubtractorVel = 0.f;
                bgsubtractorAvVel = 0.f;
            }

		} else if(m.getAddress() == "/blobserver/endFrame"){
            currentFlowId = m.getArgAsInt32(1);
            // tcout() << "end frame flow " << flowid << endl;
            if (currentFlowId==bgsubtractorFlowId) {
                bgsubtractorAvVel = (bgsubtractorCnt>0) ? bgsubtractorVel/bgsubtractorCnt : 0;
                // tcout() << "bgsubtractorCnt\t" << bgsubtractorCnt << "\tavg vel\t" << (bgsubtractorVel/bgsubtractorCnt) << endl;
            } 

		} else if(m.getAddress() == "/blobserver/bgsubtractor"){

            // parse incoming elements:  iiiiffii: id x y size vx vy age lost
            int blobid = m.getArgAsInt32(0);
            int posx = m.getArgAsInt32(1);
            int posy = m.getArgAsInt32(2);
            int size = m.getArgAsInt32(3);
            float velx = m.getArgAsFloat(4);
            float vely = m.getArgAsFloat(5);
            int age = m.getArgAsInt32(6);
            int lost = m.getArgAsInt32(7);

            bgsubtractorCnt++;
            bgsubtractorVel += sqrt( pow(velx,2) + pow(vely,2) );

            // tcout() << "bgsubtractor : " << blobid << " " << posx << "/" << posy << " " << size << " " << velx << "/" << vely << " " << age << " " << lost << endl;

		} else if(m.getAddress() == "/blobserver/stitch"){

        } else if(m.getAddress() == "/blobserver/nop"){

		} else if(m.getAddress() == "/blobserver/broadcast"){

            string var = m.getArgAsString(0);
            string source = m.getArgAsString(1);
            string cam = m.getArgAsString(2);
            float val = ofToFloat( m.getArgAsString(3) );
            if (var.compare("exposureTime") == 0) cameraExposure = val;
            tcout() << "BROADCAST" << var << ", " << source << ", " << cam << ", " << val << endl;

		} else if(m.getAddress() == "/blobserver/hog" && currentFlowId==hogFlowId){
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
			if( iter != blobs.end() ) {		
                // blob instance already exists
			} else {					
                // create new blob instance
                newBlob = true;
				blobs[blobid].id = blobid;
                tcout() << "New Blob\t" << blobid << endl;
                blobs[blobid].perspectiveMat = &this->perspectiveMat;
                // add Events, so blobs can report back to planeApp
                ofAddListener( blobs[blobid].onLost, this, &planeApp::blobOnLost );
                ofAddListener( blobs[blobid].onFreeze, this, &planeApp::blobOnFreeze );
                ofAddListener( blobs[blobid].unFreeze, this, &planeApp::blobUnFreeze );
                ofAddListener( blobs[blobid].overFreeze, this, &planeApp::blobOverFreeze );
                ofAddListener( blobs[blobid].prepareToDie, this, &planeApp::blobUnlink );
                ofAddListener( blobs[blobid].onCreate, this, &planeApp::blobOnCreate );
                ofAddListener( blobs[blobid].updatePosition, this, &planeApp::videoFollowBlob );
                ofAddListener( blobs[blobid].onSteady, this, &planeApp::blobSteady );
                ofAddListener( blobs[blobid].onSteadyReward, this, &planeApp::blobSteadyReward );
                ofAddListener( blobs[blobid].onBreakSteady, this, &planeApp::blobBreakSteady );
                ofAddListener( blobs[blobid].onEnterStage, this, &planeApp::blobEnterStage );
                ofAddListener( blobs[blobid].onLeaveStage, this, &planeApp::blobLeaveStage );
            }
            // update blob with new values
            Blob* b = &blobs.find(blobid)->second;
            b->follow(posx + blobW/2.0, posy + blobH*0.8, siteW, siteH, stageRadius);
            b->setVelocity(velx, vely);
            // b->analyze(freezeMaxVel, freezeMinTime, freezeMaxTime, movingThr);    //
            b->age = age;
            b->lostDuration = lost;
            if (newBlob) ofNotifyEvent( blobs[blobid].onCreate, blobid, &blobs[blobid] );
//			tcout() << "blob " << blobid << "   " << posx << "|" << posy << "   - lifetime: " << b->lifetime << "\n";
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
        // ofTranslate(0, 1080);
        // ofRotateZ(-90);

        this->drawScreen(0, 0, 1);

        string instruction = scenes[scene].instructions[language][segment];
        ofFill(); ofSetColor(255);
        fontBg.drawString(instruction, 50, 50);

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

        this->drawRawData(offsx, offsy, 0.5);

        offsy += 260 + 10;
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


        this->drawScreen(1440, 0, 1);

        ofFill(); ofSetColor(255);
        ofDrawBitmapString(ofToString(ofGetFrameRate()), 10, 10);

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

    // nebula->draw(x,y,scale);
    ofEnableBlendMode(OF_BLENDMODE_ADD);
    bgMedia->draw(x,y,scale);

    // foreground videos
    for (vector<ofPtr<mediaElement> >::iterator it = fgMedia.begin(); it != fgMedia.end(); ++it) {
        if ((**it).blend) (**it).draw(x, y, scale);   //  
    }
    ofDisableBlendMode();

    // foreground videos, without BLENDING mode
    ofEnableAlphaBlending();
    for (vector<ofPtr<mediaElement> >::iterator it = fgMedia.begin(); it != fgMedia.end(); ++it) {
        if (!(**it).blend) (**it).draw(x, y, scale);
    }

    // draw starBridge dummies
    // extra things to draw?
    if (drawBridge && scene==1 && segment>1) {
        for(std::map<int, Blob>::iterator it = blobs.begin(); it != blobs.end(); ++it){
            Blob* b = &it->second;
            for(std::map<int, Neighbor>::iterator iter = b->neighbors.begin(); iter != b->neighbors.end(); ++iter){
                Neighbor* nb = &iter->second;
                if (nb->steadyDistance) {
                    // draw a line between blob and steady neighbor
                    ofNoFill(); ofSetColor(255);
                    float p1x = x + (offsetX + b->position.x * mapSiteW) * scale;
                    float p1y = y + (offsetY + b->position.y * mapSiteH) * scale;
                    float p2x = x + (offsetX + blobs[nb->id].position.x * mapSiteW) * scale;
                    float p2y = y + (offsetY + blobs[nb->id].position.y * mapSiteH) * scale;

                    float rot = -ofRadToDeg(atan2(p2x-p1x,p2y-p1y)) + 90;
                    float tx = bridgeX;
                    float ty = bridgeY;
                    float tz = sqrt( pow(tx,2) + pow(ty,2) );
                    float addRot = ofRadToDeg(atan2(tx,-ty));
                    float cx = p1x - tz*sin(ofDegToRad(rot+addRot));
                    float cy = p1y + tz*cos(ofDegToRad(rot+addRot));
                    float rw = ofDist(p1x, p1y, p2x, p2y) + tx*2;
                    float rh = ty*2;

                    ofLine( p1x, p1y, p2x, p2y );
                    ofPushMatrix();
                    ofTranslate(cx,cy);
                    ofRotateZ(rot);
                    ofRect( 0, 0, rw, rh );
                    ofPopMatrix();
                    // ofLine(x + b->position.x*scale, y + b->position.y*scale, x + blobs[nb->id].position.x*scale, y + blobs[nb->id].position.y*scale);
                }
            }
        }
    }


    ofDisableAlphaBlending();

    string instruction = scenes[scene].instructions[language][segment];
    ofFill(); ofSetColor(255);
    fontBg.drawString(instruction, x+20, y+projectionH*scale*0.8);

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

    string analysisTxt = scenes[scene].analysis[segment];
    ofFill(); ofSetColor(255);
    fontSm.drawString(analysisTxt, x+20, y + projectionH*scale*0.05);


    if (scene==0) {
        // IDLE mode
        // draw circle for each blob
        int bx = x + 100; int by = y + 150;
        ofFill();
        for(std::map<int, Blob>::iterator it = blobs.begin(); it != blobs.end(); ++it){
            Blob* b = &it->second;
            if (b->onStage) {
                ofSetColor(50);
                ofCircle(bx, by, 50);
                ofSetColor(0,0,255);
                ofDrawBitmapString( ofToString(b->id, 4, '0'), bx-15, by+5);
                bx += 130;
                if (bx < projectionW-300) {
                    bx = x + 100;
                    by += 110;
                }
            }
        }
    } else if (scene==1) {
        if( segment==0 || segment==1 ) {
            // STAND STILL
            // draw circle for each blob, and display frozen, frozentimer
            int bx = x + 100; int by = y + 150;
            ofFill();
            for(std::map<int, Blob>::iterator it = blobs.begin(); it != blobs.end(); ++it){
                Blob* b = &it->second;
                if (b->onStage) {
                    if(b->properFreeze) {
                        if(segment==1 && b->frozenTimer > freezeMaxTime) {
                            ofSetColor(0);
                        } else ofSetColor(255);
                    } else if(b->frozen) ofSetColor(100);
                    else ofSetColor(50);

                    ofCircle(bx, by, 50);
                    ofSetColor(0,0,255);
                    ofDrawBitmapString( ofToString(b->id, 4, '0'), bx-15, by+5);

                    string textStr = "vel\t\t: " + ofToString(b->vel, 2);
                    textStr += "\nfrozen\t\t: "+ ofToString(b->frozen ? "true" : "false");
                    textStr += "\nproperFreeze \t: "+ ofToString(b->properFreeze ? "true" : "false");
                    textStr += "\noverFrozen \t: "+ ofToString(b->overFrozen ? "true" : "false");
                    textStr += "\nfrozenTimer \t: " + ofToString(b->frozenTimer);
                    ofDrawBitmapStringHighlight(textStr, bx+70, by -30);

                    by += 110;
                }
            }

        } else if( segment==2 || segment==3 ) {

            // KEEP THE DISTANCE
            int bx = x + 100; int by = y + 150;
            ofFill();
            for(std::map<int, Blob>::iterator it = blobs.begin(); it != blobs.end(); ++it){
                Blob* b = &it->second;
                if (b->onStage) {
                    if (b->movingMean) ofSetColor(255);
                    else ofSetColor(100);

                    ofCircle(bx, by, 50);
                    ofSetColor(0,0,255);
                    ofDrawBitmapString( ofToString(b->id, 4, '0'), bx-15, by+5);

                    string textStr = "neighbors\t: "+ ofToString(b->neighbors.size());
                    textStr += "\nmovingMean\t: " + ofToString(b->movingMean ? "true" : "false");
                    ofDrawBitmapStringHighlight(textStr, bx+70, by -40);

                    for(std::map<int, Neighbor>::iterator iter = b->neighbors.begin(); iter != b->neighbors.end(); ++iter){
                        Neighbor* nb = &iter->second;
                        if (nb->steadyDistance) ofSetColor(255); else ofSetColor(0);
                        ofDrawBitmapString(ofToString(nb->id), bx+70, by-5);
                        ofRect(bx+120, by+20, nb->distance[0]*0.2, -10);
                        by += 15;
                    }

                    by += 60;
                }
            }
        }
    } else if (scene==3) {
        if (segment==1 || segment==2) {

            // HOP
            int bx = x + 100; int by = y + 150;
            ofFill();
            for(std::map<int, Blob>::iterator it = blobs.begin(); it != blobs.end(); ++it){
                Blob* b = &it->second;
                if (b->onStage) {

                    if(b->lostDuration > 0 && !b->onEdge) ofSetColor(255);  // b->lostDuration < hopLength 
                    else ofSetColor(100);

                    ofCircle(bx, by, 40);
                    ofSetColor(0,0,255);
                    ofDrawBitmapString( ofToString(b->id, 4, '0'), bx-15, by+5);

                    string textStr = "onEdge\t: " + ofToString(b->onEdge ? "true" : "false");
                    textStr += "\nlost\t: "+ ofToString(b->lostDuration);
                    ofDrawBitmapStringHighlight(textStr, bx+70, by -20);

                    by += 70;
                }
            }

        }

    } else if (scene==5) {
        // SHOOTING STARS
        int bx = x + 100; int by = y + 150;
        ofFill();
        for(std::map<int, Blob>::iterator it = blobs.begin(); it != blobs.end(); ++it){
            Blob* b = &it->second;
            if (b->onStage) {

                if(b->lostDuration > 0 && !b->onEdge) ofSetColor(255);  // b->lostDuration < hopLength 
                else ofSetColor(100);

                ofCircle(bx, by, 40);
                ofSetColor(0,0,255);
                ofDrawBitmapString( ofToString(b->id, 4, '0'), bx-15, by+5);

                string textStr = "id\t: " + ofToString(b->id);
                textStr += "\nlost\t: "+ ofToString(b->lostDuration);
                textStr += "\nvel x\t: "+ ofToString(b->velocity.x);
                textStr += "\nvel y\t: "+ ofToString(b->velocity.y);
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

    string instruction = scenes[scene].instructions[language][segment];
    ofDrawBitmapString("SCENE ID\t" + ofToString(scene) +
                       "\nSCENE NAME\t" + scenes[scene].name +
                       "\nSEGMENT\t\t" + ofToString(segment) +
                       "\nLENGTH\t\t" + ofToString(scenes[scene].length[segment]) +
                       "\nSUCCESS\t\t" + ofToString(success ? "true" : "false") +
                       "\nSUCCESS CNT\t" + ofToString(successCnt) +
                       "\nACTIVITY CNT\t" + ofToString(activityCnt) +
                       "\nFG MEDIA\t" + ofToString(fgMedia.size()) +
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
    string rawInfo = "port: \t\t" + ofToString(PORT);
    rawInfo += "\nosc active: \t" + ofToString(oscActive ? "true" : "false");
    rawInfo += "\nosc last msg: \t" + ofToString(oscLastMsgTimer) + " sec";
    rawInfo += "\nexposure: \t" + ofToString(cameraExposure,5);
    rawInfo += "\nbgs blob cnt: \t" + ofToString(bgsubtractorCnt);
    rawInfo += "\navg velocity: \t" + ofToString(bgsubtractorAvVel,2);
    ofDrawBitmapStringHighlight(rawInfo, x + 3, y + blobserverH*scale + 15);

    // draw frame for each blob. blobserver frame size = 64 x 128 px
    for(std::map<int, Blob>::iterator it = blobs.begin(); it != blobs.end(); ++it){
        Blob* b = &it->second;
        ofRect( x + b->_rawPos.x*scale - blobW*scale/2.0, y + b->_rawPos.y*scale - blobH*scale*0.8, blobW*scale, blobH*scale);
    }

    // draw steles
    for (int i=0; i<8; i++) {
        if (i==0) ofCircle(x + steles[i].x*scale, y + steles[i].y*scale, 5*scale, 5*scale);
        else ofCircle(x + steles[i].x*scale, y + steles[i].y*scale, 10*scale, 10*scale);
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
        Blob* b = &it->second;
        if (b->onStage) ofSetColor(255); else ofSetColor(160);
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

    // draw steles
    ofNoFill(); ofSetColor(255);
    for (int i=0; i<8; i++) {
        if (i==0) ofCircle(x + steles_topdown[i].x*scale, y + steles_topdown[i].y*scale, 5*scale, 5*scale);
        else ofCircle(x + steles_topdown[i].x*scale, y + steles_topdown[i].y*scale, 10*scale, 10*scale);
    }
}


bool planeApp::allBlobsAlignedWith(ofPoint &p) {
    bool allAligned = true;
    for(std::map<int, Blob>::iterator it = blobs.begin(); it != blobs.end(); ++it){
        Blob* b = &it->second;
        float x = offsetX + b->position.x * mapSiteW;
        float y = offsetY + b->position.y * mapSiteH;
        float d = ofDist(x, y, p.x, p.y);
        if (d > alignmentMaxDist) {
            allAligned = false;
            break;
        }
    }
    return allAligned;
}


void planeApp::exit() {
    // do some destructing here
    tcout() << "goodbye" << endl;
}


//--------------------------------------------------------------
void planeApp::keyPressed(int key){

}

//--------------------------------------------------------------
void planeApp::keyReleased(int key){

    if (key == 'd') {
        drawBlobDetail = !drawBlobDetail;
    }

    if (key == 'b') {
        drawBridge = !drawBridge;
    }

    if (key == OF_KEY_RETURN) {
        processing = !processing;
    }

    if (key == OF_KEY_DOWN){
        bridgeX += 2;
        tcout() << "bridgeX \t" << bridgeX << endl;
    } else if (key == OF_KEY_UP){
        bridgeX -= 2;
        tcout() << "bridgeX \t" << bridgeX << endl;
    }
    if (key == OF_KEY_PAGE_DOWN){
        bridgeY += 2;
        tcout() << "bridgeY \t" << bridgeY << endl;
    } else if (key == OF_KEY_PAGE_UP){
        bridgeY -= 2;
        tcout() << "bridgeY \t" << bridgeY << endl;
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
        // fullscreen = !fullscreen;
        // if (fullscreen) {
        //     ofSetWindowPosition(1440,0);
        //     ofSetFullscreen(true);
        // } else {
        //     ofSetWindowPosition(0,0);
        //     ofSetFullscreen(false);
        // }
    }

    if (key=='p') {
        tcout() << "make screenshot" << endl;
        ofImage img;
        img.grabScreen(0,0,1425,700);
        string fileName = "plane_"+ofGetTimestampString()+".png";
        img.saveImage(fileName);
        tcout() << "saved screenshot " << fileName.c_str() << endl;
    }

    if (key>='0' && key<='5') {
        int s = int(key) - int('0');
        tcout() << "key input = " << key << " scene = " << s << endl;
        jumpToScene(s);
    }

    if (key=='g') {
        string alignmentGlow = "video/5_eclipse/ECLIPSE_alignment-glow-01-animation.mov";
        fgMedia.push_back(ofPtr<mediaElement>( new videoElement(alignmentGlow) ));
        (*fgMedia[fgMedia.size()-1]).setDisplay(projectionW/2, projectionH/2, true);
        (*fgMedia[fgMedia.size()-1]).reset(true);
        (*fgMedia[fgMedia.size()-1]).autoDestroy(true);
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
