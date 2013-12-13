#include "planeApp.h"
#include "ofBitmapFont.h"


std::ostream& tcout() {
  // Todo: get a timestamp in the desired format
  return std::cout << ofGetFrameNum() << " \t";
}

//--------------------------------------------------------------
void planeApp::setup(){

    ofSetFrameRate(30);
    ofLogNotice("START") << "\t" << ofGetFrameNum() << "\t" << "setup";

    ofSetLogLevel(OF_LOG_NOTICE);
    ofLogNotice("START") << "\t" << ofGetFrameNum() << "\t" << "setup3";
    // OF_LOG_VERBOSE
    // OF_LOG_NOTICE
    // OF_LOG_WARNING
    // OF_LOG_ERROR
    // OF_LOG_FATAL_ERROR
    // OF_LOG_SILENT

    // ofLogVerbose()
    // ofLogNotice()
    // ofLogWarning()
    // ofLogError()
    // ofLogFatalError()
    ofLogNotice("START") << "\t" << ofGetFrameNum() << "\t" << "0";

    ofSetLogLevel("BLOB", OF_LOG_VERBOSE);
    ofSetLogLevel("BRIDGE", OF_LOG_VERBOSE);
    ofSetLogLevel("TRANSITION", OF_LOG_VERBOSE);
    ofSetLogLevel("OSC", OF_LOG_WARNING);
    ofSetLogLevel("interaction", OF_LOG_VERBOSE);
    ofSetLogLevel("videoElement", OF_LOG_VERBOSE);
    ofSetLogLevel("mediaElement", OF_LOG_VERBOSE);

	ofTrueTypeFont::setGlobalDpi(72);


    fontBg.loadFont("CircularStd-Book.otf", 34, true, true);
    fontBg.setLineHeight(40.0f);
    fontBg.setLetterSpacing(1.037);
    fontBg.setSpaceSize(0.5);

    fontIdle.loadFont("CircularStd-Book.otf", 46, true, true);
    fontIdle.setLineHeight(52.0f);
    fontIdle.setLetterSpacing(1.037);
    fontIdle.setSpaceSize(0.5);

    fontSm.loadFont("Mercury Bold.otf", 20, true, true);
	fontSm.setLineHeight(16.0f);
	fontSm.setLetterSpacing(1.037);


    ofLogNotice("START") << "\t" << ofGetFrameNum() << "\t" << "1";
    projectorOn = false;
    language = 1;
    processing = true;
    oscMsgReceived = false;
    oscLastMsg = 0;
    oscLastMsgTimer = 0;
    oscLastMsgTimerMax = 10;
    oscActive = false;
	mouseX = 0;
	mouseY = 0;
	mouseButtonState = "";
    lastActivityClock = ofGetUnixTime();
    lastActivity = 0;

    fullscreen = false;
    // ofSetWindowPosition(0,0);
    ofLogNotice("START") << "\t" << ofGetFrameNum() << "\t" << "2";
    success = false;
    successCnt = 0;
    blobsOnStage = 0;
    activityCnt = 0;
	drawBlobDetail = false;
    transition = false;
    endedSegment = false;
    moveOn = false;
    sceneChange = false;
    segmentChange = 1;
    flash = false;
    flashCnt = 0;
    flashMax = 20;
    bgMediaId = 0;
    pickPlanet = 0;

    drawBridge = false;
    bridgeX = 5;
    bridgeY = 55;

    projectionW = 1200;
    projectionH = 1920;
    projectionOffsetX = 60;
    blobserverW = 500;
    blobserverH = 300;
    blobW = 80;
    blobH = 160;

    hogFlowId = 1;
    hogFlowName = "Actuator";
    bgsubtractorFlowId = 2; // 1 = hog, 2 = bgs, 3 = nop
    bgsubtractorCnt = 0;
    bgsubtractorVel = 0.f;
    bgsubtractorAvVel = 0.f;
    hogAvVel = 0.f;

    followMe = -0.5;
    shootingPointer = 0;

    siteW.addListener(this,&planeApp::recalculatePerspective);
	siteH.addListener(this,&planeApp::recalculatePerspective);

    ofLogNotice("START") << "\t" << ofGetFrameNum() << "\t" << "3";

    if (!projectorOn) gui.setup("HUMANS AND PLANETS", "planets01.xml", 1104,190);
    else gui.setup("HUMANS AND PLANETS", "planets01.xml", 1604,190);
    gui.setDefaultBackgroundColor( ofColor(0,0,50) );
    gui.add(autoplay.setup("autoplay", false));
    gui.add(testMode.setup("testMode", false));
    gui.add(displayDebug.setup("displayDebug", false));
    gui.add(minLostTime.set("Min LostTime", 1, 0, 10));
    gui.add(inactivityTimer.set("inactivity timer", 30, 10, 200));
    gui.add(minSegmentLength.set("Min Segm Length", 3, 0, 10));
    gui.add(nebulaOpacity.set("Nebula Opacity", 50, 0, 100));
    gui.add(flashColor.set("Transition Flash Color",ofColor(255,200,100),ofColor(0,0),ofColor(255,255)));

    paramBasic.setName("Dimension");
    paramBasic.add(siteW.set( "Site Width", 500, 0, 1000));
    paramBasic.add(siteH.set( "Site Length", 500, 0, 1000));
    paramBasic.add(stageRadius.set( "Stage Radius", 200, 0, 500));
    paramBasic.add(mapSiteW.set( "Map Width", 2, 0, 4));
    paramBasic.add(mapSiteH.set( "Map Height", 2, 0, 4));
    paramBasic.add(offsetX.set( "Offset X", 0, -500, 500));
    paramBasic.add(offsetY.set( "Offset Y", 0, -500, 1000));
    paramBasic.add(y_mean.set( "Y Interpolation", 5, 1, 15));
    gui.add(paramBasic);

    paramBlob.setName("Blobserver");
    gui.add(configBlobserver.setup("configBlobserver", false));
    paramBlob.add(noiseSlow.set( "SLOW noise", 9, 0, 10));
    paramBlob.add(measurementSlow.set( "SLOW measurement", 6, 0, 10));
    paramBlob.add(noiseNormal.set( "NORMAL noise", 7, 0, 10));
    paramBlob.add(measurementNormal.set( "NORMAL measurement", 4, 0, 10));
    paramBlob.add(noiseErratic.set( "ERRATIC noise", 3, 0, 10));
    paramBlob.add(measurementErratic.set( "ERRATIC measurement", 7, 0, 10));
    gui.add(paramBlob);
    // paramTiming.setName("Timing");
    // gui.add(paramTiming);

    paramSc1.setName("Sc1 Stars");
    paramSc1.add(freezeMaxVel.set( "Freeze MaxVel",0.1, 0, 3.0 ));
    paramSc1.add(freezeMinTime.set( "Freeze MinTime",2, 0, 5 ));
    paramSc1.add(freezeMaxTime.set( "Freeze MaxTime",10, 0, 30 ));
    paramSc1.add(newStarMax.set( "NewStar Max", 30, 1, 40));
    // paramSc1.add(newStarBonus.set( "Bonus every", 5, 1, 40));
    paramSc1.add(distStdDevThr.set( "Dist StdDev", 10, 0, 50));
    paramSc1.add(movingThr.set( "Movement Thr", 0.1, 0, 3));
    paramSc1.add(steadyRewardTime.set( "Dist Reward", 2, 0, 10));
    gui.add(paramSc1);

    paramSc2.setName("Sc2 Revolutions");
    paramSc2.add(minLostSpin.set("MinLost SPIN", 1, 0, 10));
    paramSc2.add(spinJudgeTime.set( "spinJudgeTime", 180, 0, 240));
    paramSc2.add(spinSuccess.set( "spinSuccess", 5, 0, 10));
    paramSc2.add(spinFailure.set( "spinFailure", 2, 0, 10));
    gui.add(paramSc2);

    paramSc3.setName("Sc3 Sun");
    // paramSc3.add(edgeMargin.set( "Edge Margin", 50, 0, 150));
    paramSc3.add(minLostHop.set("MinLost HOP", 1, 0, 10));
    paramSc3.add(activityColorCh.set( "Activity Color Change", 10, 0, 30));
    paramSc3.add(freezeAllMaxVel.set( "FreezeAllMaxVel",0.1, 0, 3.0 ));
    // paramSc3.add(freezeJudgeTime.set( "freezeJudgeTime", 180, 0, 240));
    // paramSc3.add(freezeVideoSpeedMap.set( "freezeVideoSpeedMap", 0.1, 0, 1));
    paramSc3.add(runJudgeTime.set( "runJudgeTime", 80, 0, 240));
    paramSc3.add(runHogThr.set( "runHogThr", 1.0, 0, 10));
    paramSc3.add(runBgsThr.set( "runBgsThr", 3.0, 0, 10));
    paramSc3.add(runActThr.set( "runActThr", 5, 0, 20));
    gui.add(paramSc3);

    paramSc4.setName("Sc4 Alignment");
    paramSc4.add(alignmentMaxDist.set( "Alignm MaxDist", 50, 0, 200));
    paramSc4.add(alignmentTransition.set( "Alignm Transition", 10, 0, 20));
    paramSc4.add(followMeSpeed.set( "followMeSpeed", 0.002, 0, 0.02));
    paramSc4.add(followMeRadius.set( "followMeRadius", 300, 0, 500));
    gui.add(paramSc4);

    paramSc5.setName("Sc5 Combustion");
    paramSc5.add(minLostShoot.set("MinLost SHOOT", 1, 0, 10));
    gui.add(paramSc5);

    ofLogNotice("START") << "\t" << ofGetFrameNum() << "\t" << "4";
    gui.setSize(200,500);
    gui.loadFromFile("planets01.xml");

    // OSC SETUP
    try {
    ofLogNotice("OSC") << "\t\t" << ofGetFrameNum() << "\t" << "listening for osc messages on port " << MYPORT;
    receiver.setup(MYPORT);
    ofLogNotice("OSC") << "\t\t" << ofGetFrameNum() << "\t" << "sending osc messages to " << BLOBSERVERIP << " on port " << BLOBPORT;
    sender.setup( BLOBSERVERIP, BLOBPORT );


    // clear receiver
    while(receiver.hasWaitingMessages()){
        ofxOscMessage m;
        receiver.getNextMessage(&m);
    }

    sendOscMsg("signIn", MYIP, MYPORT);
    } catch(exception& e){
        ofLogError() << e.what();
    }


    ofLogNotice("START") << "\t" << ofGetFrameNum() << "\t" << "5";

    // init
    this->initScenes();

    instructionImg.loadImage("img/placeholder_letgo.jpg");


    // compute the perspectiveTransformation
    // to map from blob-coordinates to the top-down view
    this->setPerspective();

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


	ofBackground(255);

}

void planeApp::recalculatePerspective(int & v) {
    ofLogNotice() << "recalculatePerspective\t" << "siteW " << siteW << "\tsiteH " << siteH;
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

    ofLogNotice("START") << "\t" << ofGetFrameNum() << "\t" << "initialize scenes";

    int n = 0;

    globalStart = ofGetUnixTime();
    segmentStart = ofGetUnixTime();
    scene = -1;
	segment = 0;
	segmentClock = 0;
	masterClock = 0;
    resetClock = false;


    nebula = ofPtr<mediaElement>( new videoElement("video/NEBULA_131211-H264.mp4"));
    nebula->reset(true);
    nebula->opMax = nebulaOpacity/100.f;
    nebulaOpacity.addListener(this,&planeApp::guiNebulaChange);

    bgMedia.push_back(ofPtr<mediaElement>( new videoElement("video/IDLE_MODE_11-half-1-H264-10mbps.mp4",false)));       // 0
    bgMedia.push_back(ofPtr<mediaElement>( new videoElement("video/IDLE_MODE_13_blue-anim-H264-10mbps.mp4",false)));    // 1
    bgMedia.push_back(ofPtr<mediaElement>( new videoElement("video/IDLE_MODE_13_white-anim-H264-10mbps.mp4",false)));   // 2
    bgMedia.push_back(ofPtr<mediaElement>( new videoElement("video/IDLE_MODE_13_yellow-anim-H264-10mbps.mp4",false)));  // 3
    bgMedia.push_back(ofPtr<mediaElement>( new videoElement("video/BACKGROUND-1_intro-H264_10mpbs.mp4",false)));        // 4
    bgMedia.push_back(ofPtr<mediaElement>( new videoElement("video/BACKGROUND-1_loop-H264_10mpbs.mp4",false)));         // 5
    bgMedia.push_back(ofPtr<mediaElement>( new videoElement("video/4_sun/SUN_15-intro-photoJPEG.mov",false)));          // 6


    sceneInfo idle;
    idle.name = "Idle";
    idle.no = n;
    idle.segments = 1;
    idle.analysis[0] = "* detection";
    idle.instructions[0][0] = "Come closer";
    idle.instructions[1][0] = "Approchez";
    idle.instructions[2][0] = "Approchez";
    idle.length[0] = -1;
    scenes[n] = idle;


    n++;

    sceneInfo stars;
    stars.name = "Star Links";
    stars.no = n;
    stars.segments = 4;
    stars.instructions[0][0] = "Stand still";
    stars.instructions[1][0] = "Ne bougez plus";
    stars.instructions[2][0] = "Ne bougez plus";
    stars.analysis[0] = "* Velocity < FreezeMaxVel\n* frozenTimer > freezeMinTime\n-> 10 sec";
    stars.length[0] = 10;
    stars.instructions[0][1] = "Try new spots\nto light up more stars";
    stars.instructions[1][1] = "Posez-vous à de nouveaux endroits\npour allumer de nouvelles étoiles";
    stars.instructions[2][1] = "Posez-vous a de nouveaux endroits\npour allumer de nouvelles etoiles";
    stars.analysis[1] = "* Velocity < FreezeMaxVel\n* frozenTimer > freezeMinTime\n* frozenTimer < freezeMaxTime\n+ Star Animation at end of freezeMaxTime\n-> newStarMax stars || 40 sec";
    stars.length[1] = 40;
    stars.instructions[0][2] = "Walk with someone\nKeep the same distance\nbetween you\n(No hands!)";
    stars.instructions[1][2] = "Marchez avec quelqu'un\nGardez toujours le même écart\n(Sans les mains!)";
    stars.instructions[2][2] = "Marchez avec quelqu'un\nGardez toujours le meme ecart\n(Sans les mains!)";
    stars.analysis[2] = "* velocity history > movingThr\n* distance history < distStdDevThr\n-> 40 sec";
    stars.length[2] = 40;
    stars.instructions[0][3] = "Walk with someone\nMake eye contact\nKeep the distance";
    stars.instructions[1][3] = "Marchez avec quelqu'un\nRegardez-vous dans les yeux\nGardez toujours le même écart";
    stars.instructions[2][3] = "Marchez avec quelqu'un\nRegardez-vous dans les yeux\nGardez toujours le meme ecart";
    stars.analysis[3] = "* velocity history > movingThr\n* distance history < distStdDevThr\n-> 30 sec";
    stars.length[3] = 30;
    scenes[n] = stars;

    n++;

    sceneInfo revolution;
    revolution.name = "Revolution";
    revolution.no = n;
    revolution.segments = 2;
    revolution.instructions[0][0] = "Take someone's hand\nSpin and lean out\nas far as possible";
    revolution.instructions[1][0] = "Prenez quelqu'un par les mains\nTournez ensemble en laissant tomber\nvos têtes vers l'arrière";
    revolution.instructions[2][0] = "Prenez quelqu'un par les mains\nTournez ensemble en laissant tomber\nvos tetes vers l'arriere";
    revolution.analysis[0] = "* \n-> 20 sec";
    revolution.length[0] = 20;
    revolution.instructions[0][1] = "";
    revolution.instructionImg[0][1] = "LET-GO-BLACK.jpg";
    revolution.instructions[1][1] = "";
    revolution.instructions[2][1] = "";
    revolution.instructionImg[1][1] = "LACHEZ-TOUT-BLACK.jpg";
    revolution.analysis[1] = "* \n-> 10 sec";
    revolution.length[1] = 10;
    scenes[n] = revolution;

    n++;

    sceneInfo sun;
    sun.name = "Explosion";
    sun.no = n;
    sun.segments = 5;
    sun.instructions[0][0] = "Stand on one leg";
    sun.instructions[1][0] = "Tenez-vous sur une jambe";
    sun.instructions[2][0] = "Tenez-vous sur une jambe";
    sun.analysis[0] = "- \n-> 12 sec";
    sun.length[0] = 12;
    sun.instructions[0][1] = "Hop from one\nspot to the other";
    sun.instructions[1][1] = "Déplacez-vous dans l'espace\nen sautant";
    sun.instructions[2][1] = "Deplacez-vous dans l'espace\nen sautant";
    sun.analysis[1] = "* onLost event\n-> 20 sec";
    sun.length[1] = 20;
    // sun.instructions[0][2] = "Everyone in unison";
    // sun.instructions[1][2] = "Tout le monde à l'unisson";
    // sun.analysis[2] = "* onLost event\n-> 20 sec";
    // sun.length[2] = 20;
    sun.instructions[0][2] = "everyone";
    sun.instructionImg[0][2] = "FREEZE-BLACK.jpg";
    sun.instructions[1][2] = "tout le monde";
    sun.instructions[2][2] = "tout le monde";
    sun.instructionImg[1][2] = "STOP-BLACK.jpg";
    sun.analysis[2] = "* velocity < freezeMaxVel\n-> 20 sec || all frozen";
    sun.length[2] = 20;
    sun.instructions[0][3] = "Run in every\ndirection at once.";
    sun.instructions[1][3] = "Courez partout en même temps";
    sun.instructions[2][3] = "Courez partout en meme temps";
    sun.analysis[3] = "* \n-> 40 sec";
    sun.length[3] = 40;
    sun.instructions[0][4] = "everyone";
    sun.instructionImg[0][4] = "FREEZE-BLACK.jpg";
    sun.instructions[1][4] = "tout le monde";
    sun.instructions[2][4] = "tout le monde";
    sun.instructionImg[1][4] = "STOP-BLACK.jpg";
    sun.analysis[4] = "* velocity < freezeMaxVel\n-> 20 sec || all frozen";
    sun.length[4] = 13;
    scenes[n] = sun;

    n++;

    sceneInfo eclipse;
    eclipse.name = "Alignment";
    eclipse.no = n;
    eclipse.segments = 7;
    eclipse.instructions[0][0] = "Now line-up in\nfront of me";
    eclipse.instructions[1][0] = "Faites une file face à moi";
    eclipse.instructions[2][0] = "Faites une file face a moi";
    eclipse.analysis[0] = "- \n-> 20 sec";
    eclipse.length[0] = 20;
    eclipse.instructions[0][1] = "Follow me";
    eclipse.instructions[1][1] = "Suivez moi";
    eclipse.instructions[2][1] = "Suivez moi";
    eclipse.analysis[1] = "\n-> 22 sec";
    eclipse.length[1] = 22;
    eclipse.instructions[0][2] = "Step out of the line";
    eclipse.instructions[1][2] = "Sortez de la file";
    eclipse.instructions[2][2] = "Sortez de la file";
    eclipse.analysis[2] = "\n-> 10 sec || out of line";
    eclipse.length[2] = 10;
    eclipse.instructions[0][3] = "Step into the line";
    eclipse.instructions[1][3] = "Revenez dans la file";
    eclipse.instructions[2][3] = "Revenez dans la file";
    eclipse.analysis[3] = "* x == main.x\n-> 10 sec || in line";
    eclipse.length[3] = 10;
    eclipse.instructions[0][4] = "Step out of the line";
    eclipse.instructions[1][4] = "Sortez de la file";
    eclipse.instructions[2][4] = "Sortez de la file";
    eclipse.analysis[4] = "\n-> 10 sec || out of line";
    eclipse.length[4] = 10;
    eclipse.instructions[0][5] = "Step into the line";
    eclipse.instructions[1][5] = "Revenez dans la file";
    eclipse.instructions[2][5] = "Revenez dans la file";
    eclipse.analysis[5] = "* x == main.x\n-> 10 sec || in line";
    eclipse.length[5] = 10;
    eclipse.instructions[0][6] = "Disperse very slowly\ntowards the edges";
    eclipse.instructions[1][6] = "Dispersez-vous\ntrès lentement";
    eclipse.instructions[2][6] = "Dispersez-vous\ntres lentement";
    eclipse.analysis[6] = "* edge-proximity = opacity\n-> 15 sec";
    eclipse.length[6] = 15;
    scenes[n] = eclipse;

    n++;

    sceneInfo shooting;
    shooting.name = "Combustion";
    shooting.no = n;
    shooting.segments = 3;
    shooting.instructions[0][0] = "Move like a\nshooting star";
    shooting.instructions[1][0] = "Filez comme une étoile";
    shooting.instructions[2][0] = "Filez comme une etoile";
    shooting.analysis[0] = "* onLost event\n-> 25 sec";
    shooting.length[0] = 25;
    shooting.instructions[0][1] = "Drop to the\nground";
    shooting.instructions[1][1] = "Laissez-vous tomber au sol";
    shooting.instructions[2][1] = "Laissez-vous tomber au sol";
    shooting.analysis[1] = "* onLost event\n-> 15 sec";
    shooting.length[1] = 15;
    shooting.instructions[0][2] = "";
    shooting.instructionImg[0][2] = "look-at-the-sky-BLACK.jpg";
    shooting.instructions[1][2] = "";
    shooting.instructions[2][2] = "";
    shooting.instructionImg[1][2] = "regardez-le-ciel-BLACK.jpg";
    shooting.analysis[2] = "- \n-> 15 sec";
    shooting.length[2] = 15;
    shooting.instructions[0][3] = "Stand up";
    shooting.instructions[1][3] = "Relevez-vous";
    shooting.instructions[2][3] = "Relevez-vous";
    shooting.analysis[3] = "- \n-> 10 sec";
    shooting.length[3] = 10;
    scenes[n] = shooting;

    ofLogNotice("START") << "\t" << ofGetFrameNum() << "\t" << "there are " << scenes.size() << " scenes";

    sceneChange = true;
    nextSegment(1);

}

//--------------------------------------------------------------
void planeApp::update(){

    if (processing) {

        oscMsgReceived = false;
        this->receiveOsc();

        //if () {
        oscLastMsgTimer = ofGetElapsedTimef() - oscLastMsg;
        if (!oscMsgReceived && oscActive && oscLastMsgTimer > oscLastMsgTimerMax) {
            oscActive = false;
        }
        //}

        if (oscMsgReceived) {

        	// BLOB CLEANUP
            std::map<int,Blob>::iterator it = blobs.begin();
            while (it != blobs.end()) {
                Blob* b = &it->second;

                int minLost = minLostTime;
                if (scene==2) minLost = minLostSpin;
                else if (scene==3) minLost = minLostHop;
                else if (scene==5) minLost = minLostShoot;

            	b->update(minLost);
            	if( !b->isAlive() ) {
                    ofLogNotice("BLOB") << "\t\t" << ofGetFrameNum() << "\t" << "dead\t" << b->id;
                    ofNotifyEvent(b->prepareToDie,b->id,b);
            		blobs.erase(it++);
            	} else {
            		++it;
            	}
            }

            // ANALYSIS
            // std::map<int, ofPoint> blobPositions;
            // for(std::map<int, Blob>::iterator it = blobs.begin(); it != blobs.end(); ++it){
            //     Blob* b = &it->second;
            //     blobPositions[b->id] = b->position;
            // }
            std::map<int, tinyNeighbor> blobPositions;
            for(std::map<int, Blob>::iterator it = blobs.begin(); it != blobs.end(); ++it){
                Blob* b = &it->second;
                blobPositions[b->id].set(b->position, b->movingMean, b->onStage);
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
            float totalVel = 0;
            for(std::map<int, Blob>::iterator it = blobs.begin(); it != blobs.end(); ++it){
                Blob* b = &it->second;
                if (b->onStage) {
                    blobsOnStage++;
                    totalVel += sqrt( pow(b->velocity.x,2) + pow(b->velocity.y,2) );
                }
            }
            if (blobsOnStage>0) lastActivityClock = ofGetUnixTime();
            hogAvVel = (blobsOnStage>0) ? totalVel/blobsOnStage : 0;

            it = blobs.begin();
            while (it != blobs.end()) {
                Blob* b = &it->second;
                b->updateVideo();
                it++;
            }
        }

        // SCHEDULING
        masterClock = ofGetUnixTime() - globalStart;
        segmentClock = ofGetUnixTime() - segmentStart;
        lastActivity = ofGetUnixTime() - lastActivityClock;
        if (flash) {
            flashCnt++;
            if (flashCnt > flashMax) {
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
                ofLogNotice("interaction") << "\t" << ofGetFrameNum() << "\t" << "scene 0 success";
                endSegment(1);
            }
        } else if (scene==1) {
            if (autoplay && segment==1 && successCnt > newStarMax && !transition) {
                ofLogNotice("interaction") << "\t" << ofGetFrameNum() << "\t" << "scene 1 segment 1 success";
                endSegment(1);
            }
        } else if (scene==2 && segment==1) {
            if (autoplay && success && !transition) {
                ofLogNotice("interaction") << "\t" << ofGetFrameNum() << "\t" << "scene 2 segment 1 success";
                endSegment(1);
            }
        } else if (scene==3) {
            if (segment==2 || segment==4 ) {
                // FREEZE!
                if (hogAvVel < freezeAllMaxVel && segmentClock > 3) {
                    success = true;
                } else {
                    success = false;
                }
                if(autoplay && success && !transition) {
                    ofLogNotice("interaction") << "\t" << ofGetFrameNum() << "\t" << "FREEZE success";
                    endSegment(1);
                }
            }
        } else if (scene==4) {
            if (autoplay && success && !transition && segmentClock > minSegmentLength ) {
                if (segment==2 || segment==4) {
                    // STEP OUT OF THE LINE
                    if (blobsOnStage > 0 && !success) endSegment(1);
                } else if (segment==3 || segment==5) {
                    // STEP INTO THE LINE
                    if (success) endSegment(1);
                }
            }
        }


        // bool found = false;
        // for (vector<ofPtr<mediaElement> >::iterator it = fgMedia.begin(); it != fgMedia.end(); it++) {
        //     if ((**it).bridgeVideo && (**it).bridgeBlobID[0]==pair.blob1 && (**it).bridgeBlobID[1]==pair.blob2) {
        //         string videoFile = "video/2_stars/LINK_01-outro-photoJPEG.mov";
        //         if ((**it).file != videoFile) {
        //             ofLogNotice("BLOB") << "\t\t" << ofGetFrameNum() << "\t" << "\t\t\tunlinked bridge";
        //             // (**it).dead = true;
        //             found = true;
        //             (**it).loadMovie("video/2_stars/LINK_01-outro-photoJPEG.mov");
        //             (**it).reset(true);
        //             (**it).autoDestroy(true);
        //         }
        //     }
        // }


        // animation related
        if (scene==2) {


            // SPIN
            // if (!transition && ofGetFrameNum()%spinJudgeTime==0) {
            //     if (segment==0) {
            //         int oldCnt = planetCnt;
            //         // set planetCnt based on activityCnt
            //         if (activityCnt >= spinSuccess) planetCnt++;
            //         else if (activityCnt < spinFailure) planetCnt--;
            //         if (activityCnt==0) planetCnt=0;
            //         if (planetCnt>5) planetCnt = 5;
            //         if (planetCnt>blobsOnStage/2) planetCnt = blobsOnStage/2;
            //         else if (planetCnt<0) planetCnt = 0;
            //         if (oldCnt==0 && planetCnt==1 && !resetClock) {
            //             // first pair spinning, reset timer!
            //             resetClock = true;
            //             segmentStart = ofGetUnixTime();
            //             ofLogNotice("interaction") << "\t" << ofGetFrameNum() << "\t" << "\treset segmentClock to " << segmentClock;
            //         }
            //         if (oldCnt!=planetCnt) positionRevolutions();
            //     } else if (segment==1) {
            //         int oldCnt = planetCnt;
            //         if (activityCnt < spinFailure) planetCnt--;
            //         if (planetCnt<oldCnt && planetCnt>=0) {
            //             (*fgMedia[planetCnt]).loadMovie("video/3_revolution/REV_OUT-photoJPEG.mov");
            //             (*fgMedia[planetCnt]).reset(true);
            //             (*fgMedia[planetCnt]).autoDestroy(true);
            //         } else if (planetCnt==-1) {
            //             success = true;
            //         }
            //     }
            //     ofLogNotice("interaction") << "\t" << ofGetFrameNum() << "\t" << "\tactivityCnt\t" << activityCnt << "\tplanetCnt\t" << planetCnt;
            //     activityCnt = 0;
            // }

            // SPIN TAKE 2


            if (!transition && ofGetFrameNum()%spinJudgeTime==0) {
                ofLogNotice("interaction") << "\t" << ofGetFrameNum() << "\t" << "\tspinJudgeTime";
                if (segment==0) {
                    int oldCnt = planetCnt;
                    // set planetCnt based on activityCnt
                    if (activityCnt >= spinSuccess) planetCnt++;
                    else if (activityCnt < spinFailure) planetCnt--;

                    if (activityCnt==0) planetCnt=0;
                    if (planetCnt>blobsOnStage/2) planetCnt = blobsOnStage/2;
                    if (planetCnt>5) planetCnt = 5;
                    else if (planetCnt<0) planetCnt = 0;

                    if (oldCnt==0 && planetCnt==1 && !resetClock) {
                        // first pair spinning, reset timer!
                        resetClock = true;
                        segmentStart = ofGetUnixTime();
                        ofLogNotice("interaction") << "\t" << ofGetFrameNum() << "\t" << "\treset segmentClock to " << segmentClock;
                    }

                    if (oldCnt!=planetCnt) {

                        if (planetCnt>oldCnt) {
                            // ADD PLANETS
                            string videoFile;
                            int videoPick = ofRandom(5) + 1;
                            videoFile = "video/3_revolution/REV_0"+ofToString(videoPick)+"-photoJPEG.mov";
                            fgMedia.push_back(ofPtr<mediaElement>( new videoElement(videoFile,true)));
                            (*fgMedia[fgMedia.size()-1]).reset(true);
                        } else {
                            ofLogNotice("interaction") << "\t" << ofGetFrameNum() << "\t" << "\ttake away planet";
                            // TAKE AWAY PLANETS
                            if (fgMedia.size()>0) {
                                for (unsigned int i=fgMedia.size()-1; i>=0; i--) {
                                    ofLogNotice("interaction") << "\t" << ofGetFrameNum() << "\t" << "\ttake away planet " << i;
                                    if (!(*fgMedia[i]).selfdestroy) {
                                        (*fgMedia[i]).loadMovie("video/3_revolution/REV_OUT-photoJPEG.mov");
                                        (*fgMedia[i]).reset(true);
                                        (*fgMedia[i]).autoDestroy(true);
                                        if (i>=planetCnt) break;
                                    }
                                }
                            }
                        }

                        positionRevolutions();
                    }

                } else if (segment==1) {
                    int oldCnt = planetCnt;
                    if (activityCnt < spinFailure) planetCnt--;
                    if (planetCnt<oldCnt && planetCnt>=0) {
                        ofLogNotice("interaction") << "\t" << ofGetFrameNum() << "\t" << "\ttake away planet";
                        if (fgMedia.size()>0) {
                            for (unsigned int i=fgMedia.size()-1; i>=0; i--) {
                                ofLogNotice("interaction") << "\t" << ofGetFrameNum() << "\t" << "\ttake away planet " << i;
                                if (!(*fgMedia[i]).selfdestroy) {
                                    (*fgMedia[i]).loadMovie("video/3_revolution/REV_OUT-photoJPEG.mov");
                                    (*fgMedia[i]).reset(true);
                                    (*fgMedia[i]).autoDestroy(true);
                                    if (i>=planetCnt) break;
                                }
                            }
                        }
                        if (planetCnt==0) success = true;
                    } else if (planetCnt==-1) {
                        success = true;
                    }
                }
                ofLogNotice("interaction") << "\t" << ofGetFrameNum() << "\t" << "\tactivityCnt\t" << activityCnt << "\tplanetCnt\t" << planetCnt;
                activityCnt = 0;
            }


        } else if (scene==3 && (segment==2 || segment==4)) {
            // FREEZE
            // if (!transition && ofGetFrameNum()%freezeJudgeTime==0) {
            //     // hogAvVel influences displaySpeed of sun-movie
            //     float videoSpeed = (hogAvVel<0.1) ? 0.1 : 0.1+hogAvVel*freezeVideoSpeedMap;
            //     (*fgMedia[0]).movie->setSpeed(videoSpeed);
            //     ofLogNotice("interaction") << "\t" << ofGetFrameNum() << "\t" << "\tfreezeJudge:\thogAvVel " << hogAvVel << " (" << blobsOnStage << ")\tvideoSpeed " << videoSpeed;
            // }
            // if (hogAvVel < freezeAllMaxVel) {
            //     success = true;
            // } else {
            //     success = false;
            // }
        } else if (scene==3 && segment==3) {
            if (!transition && ofGetFrameNum()%runJudgeTime==0) {
                // hogAvVel, bgsubtractorAvVel, activityCnt
                // SUN_run_surface-8-blue-qtPNG.mov
                // SUN_run_loop-blue_background-1-qtPNG.mov
                if (hogAvVel > runHogThr || bgsubtractorAvVel > runBgsThr || activityCnt > runActThr) {
                    // activity!
                    // ADD BLUE BACKGROUND
                    // fgMedia.push_back(ofPtr<mediaElement>( new videoElement("video/4_sun/SUN_run_loop-blue_background-1-qtPNG.mov",false)));
                    // (*fgMedia[fgMedia.size()-1]).setDisplay(projectionW/2,projectionH/2, true);
                    // (*fgMedia[fgMedia.size()-1]).reset();
                    // (*fgMedia[fgMedia.size()-1]).autoDestroy(true);
                    int rndSun = ofRandom(12)+1;
                    fgMedia.push_back(ofPtr<mediaElement>( new videoElement("video/4_sun/SUN_run_surface-"+ofToString(rndSun)+"-blue-qtPNG.mov",false)));
                    (*fgMedia[fgMedia.size()-1]).setDisplay(projectionW/2,projectionH/2, true);
                    (*fgMedia[fgMedia.size()-1]).reset();
                    (*fgMedia[fgMedia.size()-1]).autoDestroy(true);
                    ofLogNotice("interaction") << "\t" << ofGetFrameNum() << "\t" << "\trunJudge:  activity!  hog: "<< hogAvVel << "\tbg: " << bgsubtractorAvVel << "\tact: " << activityCnt;
                } else {
                    ofLogNotice("interaction") << "\t" << ofGetFrameNum() << "\t" << "\trunJudge:  not enough activity";
                }

                activityCnt = 0;
            }
        // } else if (scene==3 && segment==5) {
        //     if (ofRandom(100)<2) (*fgMedia[0]).bounce();
        } else if (scene==4 && segment==1) {
            // FOLLOW ME
            followMe += followMeSpeed;
            if (followMe >= 2*3.14) {
                (*fgMedia[0]).position.x = projectionW/2;
                if (followMe > 2*3.6 && autoplay && !transition) {
                    endSegment(1);
                }
            } else if (followMe > 0) {
                (*fgMedia[0]).position.x = projectionW/2 + sin(followMe) * followMeRadius;
            }
        }
        if ( scene==4 && segment<6 && !transition && segmentClock > minSegmentLength && blobsOnStage>0) {

            // check if all are aligned
            bool allAligned = allBlobsAlignedWith((*fgMedia[0]).position);

            if (allAligned) {
                if (!success) {
                    // aligned
                    success = true;
                    string alignmentGlow = "video/5_eclipse/LIGHT-photoJPEG.mov";
                    fgMedia.push_back(ofPtr<mediaElement>( new videoElement(alignmentGlow, true) ));
                    (*fgMedia[fgMedia.size()-1]).setDisplay((*fgMedia[0]).position.x, (*fgMedia[0]).position.y, true);
                    (*fgMedia[fgMedia.size()-1]).reset(true);
                    (*fgMedia[fgMedia.size()-1]).autoDestroy(true);
                }
            } else {
                success = false;
            }
        }

        // VIDEO
        nebula->update();
        (*bgMedia[bgMediaId]).update();

        vector<ofPtr<mediaElement> >::iterator iter = fgMedia.begin();
        while (iter != fgMedia.end()) {
            // check if video is BRIDGE and if both blobs are still alive, else release
            if ((**iter).bridgeVideo) {
                int blob1 = (**iter).bridgeBlobID[0];
                int blob2 = (**iter).bridgeBlobID[1];
                // ofLogNotice("BLOB") << "\t\t" << ofGetFrameNum() << "\t" << "bridgeVideo " << blob1 << " / " << blob2;

                std::map<int,Blob>::iterator iter1 = blobs.find(blob1);
                if( iter1 == blobs.end() ) {
                    // blob instance doesnt exist
                    ofLogVerbose("BRIDGE") << "\t" << ofGetFrameNum() << "\t" << "DEAD BRIDGE blob 1: " << blob1 << "-" << blob2;
                } else {
                    if (blobs[blob1].position.x == 0) {
                        ofLogVerbose("BRIDGE") << "\t" << ofGetFrameNum() << "\t" << "BRIDGE BLOB AT 0 X -  blob 1: " << blob1 << "-" << blob2;
                    }
                }
                std::map<int,Blob>::iterator iter2 = blobs.find(blob2);
                if( iter2 == blobs.end() ) {
                    // blob instance doesnt exist
                    ofLogVerbose("BRIDGE") << "\t" << ofGetFrameNum() << "\t" << "DEAD BRIDGE blob 2: " << blob1 << "-" << blob2;
                } else {
                    if (blobs[blob2].position.x == 0) {
                        ofLogVerbose("BRIDGE") << "\t" << ofGetFrameNum() << "\t" << "BRIDGE BLOB AT 0 X -  blob 2: " << blob1 << "-" << blob2;
                    }
                }
            }

            (**iter).update();
            if ((**iter).dead) {
                ofLogNotice("videoElement") << ofGetFrameNum() << "\t" << "delete video " << (**iter).file;
                iter = fgMedia.erase(iter);
            } else {
                ++iter;
            }

        }

    }

}

/* define the current bg videofile, depending on scene and segment */
void planeApp::bgMediaSwap(int & trans) {

    if (scene==0 && ((*bgMedia[bgMediaId]).id==-1 || (*bgMedia[bgMediaId]).id==5)) {
        ofLogNotice("TRANSITION") << "\t" << ofGetFrameNum() << "\t" << "bgMediaSwap\t\t\tidle " << trans;
        bgMediaId = ofRandom(4);
        (*bgMedia[bgMediaId]).id = bgMediaId;
        (*bgMedia[bgMediaId]).reset(true);   // play right away
        (*bgMedia[bgMediaId]).movieEndTrigger=false;
        (*bgMedia[bgMediaId]).outroTransformation = &mediaElement::finishMovie;
        ofAddListener( (*bgMedia[bgMediaId]).fadeOutEnd, this, &planeApp::bgMediaSwap );
    } else if (scene==0 && (*bgMedia[bgMediaId]).id<4) {
        ofLogNotice("TRANSITION") << "\t" << ofGetFrameNum() << "\t" << "bgMediaSwap\t\t\tstarry intro " << trans;
        bgMediaId = 4;
        (*bgMedia[bgMediaId]).id = bgMediaId;
        (*bgMedia[bgMediaId]).reset(true);
        (*bgMedia[bgMediaId]).finishMovie(1.0);
        // (*bgMedia[bgMediaId]).movieEndTrigger=true;
        ofAddListener( (*bgMedia[bgMediaId]).fadeOutEnd, this, &planeApp::bgMediaSwap );
    } else if (scene==2 && (*bgMedia[bgMediaId]).id==5) {
        ofLogNotice("TRANSITION") << "\t" << ofGetFrameNum() << "\t" << "bgMediaSwap\t\t\tsun intro " << trans;
        bgMediaId = 6;
        (*bgMedia[bgMediaId]).id = bgMediaId;
        (*bgMedia[bgMediaId]).reset(true);
        (*bgMedia[bgMediaId]).finishMovie(1.0);
        // (*bgMedia[bgMediaId]).movieEndTrigger=true;
        ofAddListener( (*bgMedia[bgMediaId]).fadeOutEnd, this, &planeApp::bgMediaSwap );
    } else {
        if (trans!=-2) nextSegment(1);
        ofLogNotice("TRANSITION") << "\t" << ofGetFrameNum() << "\t" << "bgMediaSwap\t\t\tstarry bg " << trans;
        bgMediaId = 5;
        (*bgMedia[bgMediaId]).id = bgMediaId;
        (*bgMedia[bgMediaId]).reset(true);
        (*bgMedia[bgMediaId]).movieEndTrigger=false;
        ofRemoveListener( (*bgMedia[bgMediaId]).fadeOutEnd, this, &planeApp::bgMediaSwap );
    }

}

void planeApp::allFaded(int & trans) {
    ofLogNotice("TRANSITION") << "\t" << ofGetFrameNum() << "\t" << "allFaded, moveOn = true";
    // set moveOn to true, instead of calling nextSegment()
    // to avoid conflicting threading
    moveOn = true;
}

// trans -1 : trigger first fade from main-loop and not from faded-event
void planeApp::fgMediaFadedOut(int & trans) {
    ofLogNotice("TRANSITION") << "\t" << ofGetFrameNum() << "\t" << "fgMediaFadedOut  " << trans << "  fgmedia size = " << fgMedia.size();

    if (fgMedia.size()>0) {

        if (trans == -1) {
            // first call of function, therefore we fade the last fgMedia object
            ofAddListener( (*fgMedia.back()).fadeOutEnd, this, &planeApp::fgMediaFadedOut );
            ofLogNotice("TRANSITION") << "\t" << ofGetFrameNum() << "\t" << "call outroTransformation() on fgMedia[" << (fgMedia.size()-1) << "]";
            ((*fgMedia.back()).*((*fgMedia.back()).outroTransformation))();

        } else {
            // delete last fgMedia object
            (*fgMedia.back()).dead = true;

            if (fgMedia.size()>1) {
                // fade new last fgMedia object
                ofAddListener( (*fgMedia[fgMedia.size()-2]).fadeOutEnd, this, &planeApp::fgMediaFadedOut );
                ofLogNotice("TRANSITION") << "\t" << ofGetFrameNum() << "\t" << "call outroTransformation() on fgMedia[" << (fgMedia.size()-2) << "]";
                ((*fgMedia[fgMedia.size()-2]).*((*fgMedia[fgMedia.size()-2]).outroTransformation))();
            } else {
                // fade BG now
                if (scene==0) ((*bgMedia[bgMediaId]).*((*bgMedia[bgMediaId]).outroTransformation))();
                else moveOn = true;
            }
        }
    } else {
        // fade BG now
        if (scene==0) ((*bgMedia[bgMediaId]).*((*bgMedia[bgMediaId]).outroTransformation))();
        else moveOn = true;
    }
}

// void planeApp::fgMediaFadedIn(int & trans) {
//     ofLogNotice("TRANSITION") << "\t" << ofGetFrameNum() << "\t" << "fgMediaFadedIn  " << trans;
//     if (fgMedia.size() > 0) {
//         for (vector<ofPtr<mediaElement> >::iterator it = fgMedia.begin(); it != fgMedia.end(); it++) {
//             if (!(**it).visible) {
//                 ofLogNotice("TRANSITION") << "\t" << ofGetFrameNum() << "\t" << "fgMediaFadedIn   " << (**it).file;
//                 ofAddListener( (**it).fadeInEnd, this, &planeApp::fgMediaFadedIn );
//                 ((**it).*((**it).introTransformation))();
//             }
//         }
//     }
// }

// void planeApp::bgMediaFadedIn(int & trans) {
//     ofLogNotice("TRANSITION") << "\t" << ofGetFrameNum() << "\t" << "bgMediaFadedIn";
// }

void planeApp::blobOnLost(int & blobID) {
    ofLogNotice("BLOB") << "\t\t" << ofGetFrameNum() << "\t" << blobID << " just got lost";
    if (!transition && blobs[blobID].onStage) {
        if (scene==2) {
            activityCnt++;
        } else if (scene==3) {
            // SUN explosions
            activityCnt++;
            if (segment==1 && !blobs[blobID].occluded) {
                int randomExpl = ofRandom(7) + 1;
                // string videoEnd = "_fullscale-posterized-qtPNG.mov";
                // SUN_explosion-02_fullscale-blue-posterized-centered-qtPNG.mov
                string videoEnd = "_fullscale-blue-posterized-centered-qtPNG.mov";
                // if (activityCnt > activityColorCh*2) videoEnd = "_fullscale-blue-posterized-qtPNG.mov";
                // else if (activityCnt > activityColorCh) videoEnd = "_fullscale-red-posterized-qtPNG.mov";
                string newVideoName = "video/4_sun/SUN_explosion-" + ofToString(randomExpl,2,'0') + videoEnd;
                fgMedia.push_back(ofPtr<mediaElement>( new videoElement(newVideoName,false)));
                (*fgMedia[fgMedia.size()-1]).setDisplay(blobMapToScreen(blobs[blobID].position), true);
                (*fgMedia[fgMedia.size()-1]).autoDestroy(true);
                (*fgMedia[fgMedia.size()-1]).reset();
                // if (segment==2) (*fgMedia[0]).bounce(); // sun video = [0]
            }
        } else if (scene==5) {
            // SHOOTING STARS
            if (segment==0 && !blobs[blobID].occluded) {
                // int randomShooter = ofRandom(26) + 1;
                // fgMedia.push_back(ofPtr<mediaElement>( new videoElement("video/6_shooting/SSTAR_" + ofToString(randomShooter,2,'0') + "-photoJPEG.mov")));
                // (*fgMedia[fgMedia.size()-1]).setDisplay(ofRandom(projectionW-100), ofRandom(projectionH-100), true);
                // (*fgMedia[fgMedia.size()-1]).moveAcross( blobs[blobID].velocity.x, blobs[blobID].velocity.y, projectionW, projectionH, true);
                // (*fgMedia[fgMedia.size()-1]).reset();
                if (++shootingPointer>=fgMedia.size()) shootingPointer=0;
                (*fgMedia[shootingPointer]).setDisplay(ofRandom(projectionW-100), ofRandom(projectionH-100), true);
                (*fgMedia[shootingPointer]).moveAcross( blobs[blobID].velocity.x, blobs[blobID].velocity.y, projectionW, projectionH, false);
                (*fgMedia[shootingPointer]).reset();
            } else if (segment==1 && !blobs[blobID].occluded) {
                float randdeg = ofRandom(-5.f, 5.f);
                for (int i=0; i<10; i++) {
                    if (++shootingPointer >= fgMedia.size()) shootingPointer=0;
                    (*fgMedia[shootingPointer]).setDisplay(ofRandom(projectionW-100), ofRandom(projectionH-100), true);
                    (*fgMedia[shootingPointer]).moveAcross( randdeg, 45.f, projectionW, false);
                    (*fgMedia[shootingPointer]).reset();
                }
            }
        }
    }
}

void planeApp::blobSteady(Pair & pair) {
    // add particle trail video between stars
    if (!transition && scene==1 && segment>1) {
        Blob* b1 = &blobs[pair.blob1];
        Blob* b2 = &blobs[pair.blob2];

        if(b1->onStage && b2->onStage) {
            ofLogNotice("BLOB") << "\t\t" << ofGetFrameNum() << "\t" << "blobSteady() \t\t" << pair.blob1 << " + " << pair.blob2;


            // replace video with sparklier star
            ofPtr<mediaElement> vid1 = blobs[pair.blob1].mediaLink;
            ofPtr<mediaElement> vid2 = blobs[pair.blob2].mediaLink;

            if (vid1 != NULL) {
                string videoFile = "video/2_stars/STAR_"+ofToString((*vid1).id)+"-glow-photoJPEG.mov";
                if ((*vid1).file != videoFile) {
                    ofLogNotice("BLOB") << "\t\t" << ofGetFrameNum() << "\t\t\t" << "make blob " << pair.blob1 << " sparklier";
                    (*vid1).loadMovie(videoFile);
                    (*vid1).reset();
                }
            }
            if (vid2 != NULL) {
                string videoFile = "video/2_stars/STAR_"+ofToString((*vid2).id)+"-glow-photoJPEG.mov";
                if ((*vid2).file != videoFile) {
                    ofLogNotice("BLOB") << "\t\t" << ofGetFrameNum() << "\t\t\t" << "make blob " << pair.blob2 << " sparklier";
                    (*vid2).loadMovie(videoFile);
                    (*vid2).reset();
                }
            }

            // LINK_01-loop-photoJPEG.mov

            // check if bridge already exists
            bool exists = false;
            for (vector<ofPtr<mediaElement> >::iterator it = fgMedia.begin(); it != fgMedia.end(); it++) {
                if ((**it).bridgeVideo && (**it).bridgeBlobID[0]==pair.blob1 && (**it).bridgeBlobID[1]==pair.blob2) {
                    exists = true;
                    break;
                }
            }

            if (!exists) {
                ofLogNotice("BLOB") << "\t\t" << ofGetFrameNum()  << "\t\t\t" << "add video bridge";
                string newVideoName = "video/2_stars/LINK_01-loop-photoJPEG.mov";
                fgMedia.push_back(ofPtr<mediaElement>( new videoElement(newVideoName)));
                (*fgMedia[fgMedia.size()-1]).reset();
                (*fgMedia[fgMedia.size()-1]).bridge(b1->id, b2->id);
                ofNotifyEvent( blobs[pair.blob1].updatePosition, pair.blob1, &blobs[pair.blob1] );
            } else {
                ofLogNotice("BLOB") << "\t\t" << ofGetFrameNum()  << "\t\t\t" << "bridge already exists";
            }
        }
    }
}

void planeApp::blobSteadyReward(Pair & pair) {
    ofLogNotice("BLOB") << "\t\t" << ofGetFrameNum() << "\t" << "blobSteadyReward() \t" << pair.blob1 << " + " << pair.blob2;
    if (!transition && scene==1 && segment>1) {
        // ofPtr<mediaElement> vid1 = blobs[pair.blob1].mediaLink;
        // ofPtr<mediaElement> vid2 = blobs[pair.blob2].mediaLink;

        // // replace video with sparklier star
        // if (vid1 != NULL) {
        //     ofLogNotice("BLOB") << "\t\t" << ofGetFrameNum() << "\t" << "blob " << pair.blob1 << " \t\tfound vid";
        //     (*vid1).loadMovie("video/2_stars/ATTRACTION_star_glow-01-animation.mov");
        //     (*vid1).reset();
        // }
        // if (vid2 != NULL) {
        //     ofLogNotice("BLOB") << "\t\t" << ofGetFrameNum() << "\t" << "blob " << pair.blob2 << " \t\tfound vid";
        //     (*vid2).loadMovie("video/2_stars/ATTRACTION_star_glow-01-animation.mov");
        //     (*vid2).reset();
        // }
    }
}

void planeApp::blobBreakSteady(Pair & pair) {
    if (!transition && scene==1 && segment>1) {
        ofLogNotice("BLOB") << "\t\t" << ofGetFrameNum() << "\t" << "blobBreakSteady() \t" << pair.blob1 << " + " << pair.blob2;

        ofPtr<mediaElement> vid1 = blobs[pair.blob1].mediaLink;
        ofPtr<mediaElement> vid2 = blobs[pair.blob2].mediaLink;

        // replace video with normal star
        if (vid1 != NULL && !blobs[pair.blob1].steadyRewarded) {
            string videoFile = "video/2_stars/STAR_" + ofToString((*vid1).id)+"-loop-photoJPEG.mov";
            if ((*vid1).file != videoFile) {
                (*vid1).loadMovie("video/2_stars/STAR_" + ofToString((*vid1).id)+"-loop-photoJPEG.mov");
                (*vid1).reset();
            }
        }
        if (vid2 != NULL && !blobs[pair.blob2].steadyRewarded) {
            string videoFile = "video/2_stars/STAR_" + ofToString((*vid2).id)+"-loop-photoJPEG.mov";
            if ((*vid2).file != videoFile) {
                (*vid2).loadMovie("video/2_stars/STAR_" + ofToString((*vid2).id)+"-loop-photoJPEG.mov");
                (*vid2).reset();
            }
        }

        bridgeUnlink(pair);

    }
}




void planeApp::blobOnFreeze(int & blobID) {
    if (!transition) {
        ofPoint p = blobMapToScreen(blobs[blobID].position);
        ofLogNotice("BLOB") << "\t\t" << ofGetFrameNum() << "\t" << "blobOnFreeze()\t" << blobID << " at " << int(p.x) << "|" << int(p.y);
        if (scene==1) {
            if ((segment==0 || segment==1) && blobs[blobID].onStage) {
                // STAND STILL
                // create star on location of blob
                int randomStar = ofRandom(3) + 2;
                fgMedia.push_back(ofPtr<mediaElement>( new videoElement("video/2_stars/STAR_" + ofToString(randomStar)+"-intro-photoJPEG.mov")));
                // fgMedia.push_back(ofPtr<mediaElement>( new videoElement("video/2_stars/STAR_" + ofToString(randomStar)+"-loop-photoJPEG.mov")));
                blobs[blobID].mediaLink = fgMedia[fgMedia.size()-1];
                (*fgMedia[fgMedia.size()-1]).id = randomStar;   // save video-id
                (*fgMedia[fgMedia.size()-1]).loopFile = "video/2_stars/STAR_" + ofToString(randomStar)+"-loop-photoJPEG.mov";
                (*fgMedia[fgMedia.size()-1]).setDisplay(blobMapToScreen(blobs[blobID].position), true);
                (*fgMedia[fgMedia.size()-1]).reset();
                // (*fgMedia[fgMedia.size()-1]).autoDestroy(true);
                (*fgMedia[fgMedia.size()-1]).finishMovie(1.0);
                // (*fgMedia[fgMedia.size()-1]).outroTransformation = &mediaElement::scaleAway;
                if (segment==1) successCnt++;
            }
        } else if (scene==3) {
            if ((segment==2 || segment==4)) {
                // FREEZE!
                // check if all blobs are frozen
                // frozen
                // bool allFrozen = true;
                // for(std::map<int, Blob>::iterator it = blobs.begin(); it != blobs.end(); ++it){
                //     Blob* b = &it->second;
                //     if (!b->frozen && b->onStage) allFrozen = false;
                // }
                // if (allFrozen) success = true;
            }
        }
    }
}



void planeApp::blobUnFreeze(int & blobID) {
    if (!transition) {
        if (scene==1) {
            if (segment==0 || segment==1) {
                // STAND STILL
                ofLogNotice("BLOB") << "\t\t" << ofGetFrameNum() << "\t" << "blobUnFreeze()\t\t" << blobID;
                // video fade away
                // video unlink
                ofPtr<mediaElement> vid = blobs[blobID].mediaLink;
                if (vid != NULL) {
                    // vid->movie->setPaused(true);
                    vid->fadeOut(0.001, 0.5, true);
                    blobs[blobID].mediaLink = ofPtr<mediaElement>();
                }
            }
        } else if (scene==3) {
            // if ((segment==3 || segment==5)) {
            //     success = false;
            // }
        }
    }
}

void planeApp::blobOverFreeze(int & blobID) {
    if (!transition) {
        if (scene==1) {
            if (segment==1 && blobs[blobID].onStage && blobs[blobID].mediaLink != NULL) {
                string constellations[] = {
                "CONSTELLATION_1-photoJPEG.mov",    // ok seahorse
                // "CONSTELLATION_2-photoJPEG.mov",
                // "CONSTELLATION_3-photoJPEG.mov",
                // "CONSTELLATION_4-photoJPEG.mov",
                // "CONSTELLATION_5-photoJPEG.mov",
                // "CONSTELLATION_6-photoJPEG.mov",
                "CONSTELLATION_7-photoJPEG.mov",    // ok
                "CONSTELLATION_8-photoJPEG.mov",    // bold ok
                "CONSTELLATION_9-photoJPEG.mov",    // bold ok
                "CONSTELLATION_11-photoJPEG.mov",    // bold ok
                "CONSTELLATION_13-photoJPEG.mov",    // bold ok
                "CONSTELLATION_14-photoJPEG.mov",    // bold ok
                // "CONSTELLATION_15-photoJPEG.mov",    // bold ok
                "CONSTELLATION_16-photoJPEG.mov",    // bold ok
                // "CONSTELLATION_BOAT-photoJPEG.mov",
                "CONSTELLATION_CAR-photoJPEG.mov",
                "CONSTELLATION_CASSEROLLE-photoJPEG.mov",
                // "CONSTELLATION_CUBE-photoJPEG.mov",
                // "CONSTELLATION_FISH-photoJPEG.mov",
                // "CONSTELLATION_HAND-photoJPEG.mov",
                // "CONSTELLATION_HEART2-photoJPEG.mov",
                // "CONSTELLATION_HOUSE-photoJPEG.mov",
                // "CONSTELLATION_MERMAID-photoJPEG.mov",
                // "CONSTELLATION_STICKBOY-photoJPEG.mov"
                 };

                int constOrigin[][2] = {
                    {70,518},   // 1
                    // {71,159},
                    // {458,180},  // 3
                    // {882,716},
                    // {190,378},  // 5
                    // {146,238},
                    {46,54},    // 7
                    {246,84},    // 8x:264, y:84
                    {54,228},    // 8x:54, y:228
                    {448,244},    // 11x:448, y:244
                    {88,40},    // 13 x:88, y:40
                    {136,60},    // 14 x:136, y:60
                    // {96,148},    // 15 x:96, y:148
                    {38,106},    // 16 x:38, y:106
                    // {382,46},
                    {174,44},
                    {44,86},
                    // {36,74},
                    // {264,188},
                    // {564,450},
                    // {60,50},
                    // {220,332},
                    // {176,58},
                    // {120,428}
                };

                // play constellation video, as reward, position based on first star
                int randomConst = ofRandom(sizeof(constellations) / sizeof(constellations[0]));

                ofPtr<mediaElement> starVid = blobs[blobID].mediaLink;
                ofPoint p = blobMapToScreen(blobs[blobID].position);
                if (starVid != NULL) p = starVid->position;
                ofLogNotice("BLOB") << "\t\t" << ofGetFrameNum() << "\t" << "blobOverFreeze()\t\t" << blobID << " at " << int(p.x) << "|" << int(p.y);
                string bonusVideo = "video/2_stars/" + constellations[randomConst];
                fgMedia.push_back(ofPtr<mediaElement>( new videoElement(bonusVideo) ));
                if (p.x > projectionW/2) {      // flip video if necesssary
                    p.x += constOrigin[randomConst][0];
                    p.y -= constOrigin[randomConst][1];
                    (*fgMedia[fgMedia.size()-1]).w*=-1;
                } else {
                    p.x -= constOrigin[randomConst][0];
                    p.y -= constOrigin[randomConst][1];
                }
                (*fgMedia[fgMedia.size()-1]).setDisplay(p);
                (*fgMedia[fgMedia.size()-1]).reset(true);
                (*fgMedia[fgMedia.size()-1]).autoDestroy(true);

                this->blobUnlink(blobID);
            }
        }
    }
}

void planeApp::blobEnterStage(int & blobID) {
    // then assign the appropriate ones
    if (scene==0 && !transition) {
        // COME CLOSER, recognition of people == time to move on
        if (!success) success = true;

    } else if (scene==1 && !transition) {
        if (segment==2 || segment==3) {
            // KEEP THE DISTANCE
            ofLogNotice("BLOB") << "\t\t" << ofGetFrameNum() << "\t" << "blobEnterStage()\t\t" << blobID << " (star)";
            int randomStar = ofRandom(3) + 2;
            fgMedia.push_back(ofPtr<mediaElement>( new videoElement("video/2_stars/STAR_" + ofToString(randomStar)+"-loop-photoJPEG.mov")));
            blobs[blobID].mediaLink = fgMedia[fgMedia.size()-1];
            blobs[blobID].videoTrace = true;
            (*fgMedia[fgMedia.size()-1]).setDisplay(0, 0, true);
            (*fgMedia[fgMedia.size()-1]).id = randomStar;
            (*fgMedia[fgMedia.size()-1]).reset();
            (*fgMedia[fgMedia.size()-1]).fadeIn();
            (*fgMedia[fgMedia.size()-1]).outroTransformation = &mediaElement::scaleAway;
        }
    } else if (scene==4) {
        // PLANETS
        ofLogNotice("BLOB") << "\t\t" << ofGetFrameNum() << "\t" << "blobEnterStage()\t\t" << blobID << " (planet)";
        int planedId[] = { 6, 9, 13, 15, 18, 19,20, 22, 23 };
        int planets = sizeof(planedId) / sizeof(planedId[0]);
        pickPlanet++;
        if (pickPlanet >= planets) pickPlanet = 0;
        int randomPlanet = pickPlanet;
        // P_13-qtPNG.mov
        fgMedia.push_back(ofPtr<mediaElement>( new videoElement("video/5_eclipse/P_" + ofToString(planedId[randomPlanet])+"-qtPNG.mov", true)));
        blobs[blobID].mediaLink = fgMedia[fgMedia.size()-1];
        blobs[blobID].videoTrace = true;
        (*fgMedia[fgMedia.size()-1]).setDisplay(blobMapToScreen(blobs[blobID].position), true);
        (*fgMedia[fgMedia.size()-1]).reset();

        if (segment==0 && segmentClock < alignmentTransition) {
            (*fgMedia[fgMedia.size()-1]).moveInFromSide(projectionW/2,projectionH/2);
        } else {
            (*fgMedia[fgMedia.size()-1]).fadeIn(0.1);
        }
    }
}

void planeApp::blobLeaveStage(int & blobID) {
    if (!transition) {
        if (scene==1) {
            if (segment==2 || segment==3) {
                // STAR BRIDGES
                ofLogNotice("BLOB") << "\t\t" << ofGetFrameNum() << "\t" << "blobLeaveStage()\t\t" << blobID;
                this->blobUnlink(blobID);
            }
        } else if (scene==4) {
            // PLANETS
            ofLogNotice("BLOB") << "\t\t" << ofGetFrameNum() << "\t" << "blobLeaveStage()\t\t" << blobID;
            this->blobUnlink(blobID);
        }
    }
}

void planeApp::blobOnCreate(int & blobID) {

    if (!transition) {
        // first clean up, all related blob settings
        ofLogNotice("BLOB") << "\t\t" << ofGetFrameNum() << "\t" << "blobOnCreate() \t" << blobID;
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

    // if (fgMedia.size()>=5 ) {
    //     // REVOLUTION

    //     // there should only be 5 fgMedia elements
    //     for (unsigned int i=0; i<fgMedia.size(); i++) {
    //         bool prevHide = (*fgMedia[i]).hide;
    //         (*fgMedia[i]).hide = true;
    //         if (i < planetCnt) {
    //             (*fgMedia[i]).hide = false;
    //             // (*fgMedia[i]).fadeIn();
    //         } else {
    //             if (prevHide != true) {
    //                 // (*fgMedia[i]).fadeOut();
    //             }
    //         }
    //         (*fgMedia[i]).rotation = 0;
    //     }

    //     // positioning
    //     switch (planetCnt) {
    //         case 1:     (*fgMedia[0]).setDisplay(projectionW/2, projectionH/2, true);
    //                     break;
    //         case 2:     (*fgMedia[0]).setDisplay(projectionW/2, projectionH/2 - 350, true);
    //                     (*fgMedia[1]).setDisplay(projectionW/2, projectionH/2 + 350, true);
    //                     break;
    //         case 3:     (*fgMedia[0]).setDisplay(projectionW/2, projectionH/2 - 550, true);
    //                     (*fgMedia[1]).setDisplay(projectionW/2, projectionH/2, true);
    //                     (*fgMedia[2]).setDisplay(projectionW/2, projectionH/2 + 550, true);
    //                     break;
    //         case 4:     (*fgMedia[0]).setDisplay(projectionW/2-100, projectionH/2 - 550, true);
    //                     (*fgMedia[1]).setDisplay(projectionW/2+100, projectionH/2 - 200, true);
    //                     (*fgMedia[2]).setDisplay(projectionW/2-100, projectionH/2 + 200, true);
    //                     (*fgMedia[3]).setDisplay(projectionW/2+100, projectionH/2 + 550, true);
    //                     break;
    //         case 5:     (*fgMedia[0]).setDisplay(projectionW/2, projectionH/2, true);
    //                     (*fgMedia[1]).setDisplay(projectionW/2-200, projectionH/2 - 350, true);
    //                     (*fgMedia[1]).rotation = -45;
    //                     (*fgMedia[2]).setDisplay(projectionW/2+200, projectionH/2 - 350, true);
    //                     (*fgMedia[2]).rotation = 45;
    //                     (*fgMedia[3]).setDisplay(projectionW/2-200, projectionH/2 + 350, true);
    //                     (*fgMedia[3]).rotation = 45;
    //                     (*fgMedia[4]).setDisplay(projectionW/2+200, projectionH/2 + 350, true);
    //                     (*fgMedia[4]).rotation = -45;
    //                     break;
    //     }
    // }

    // SPIN TAKE 2

    if (fgMedia.size()>=1 ) {
        ofLogNotice("interaction") << "\t" << ofGetFrameNum() << "\t" << "positionRevolutions() \t planetCnt: " << planetCnt << " fgM: " << fgMedia.size();
        // REVOLUTION

        // there should only be 5 fgMedia elements
        for (unsigned int i=0; i<fgMedia.size(); i++) {
            (*fgMedia[i]).rotation = 0;
        }
        ofLogNotice("interaction") << "\t" << ofGetFrameNum() << "\t" << "positionRevolutions() \t planetCnt: " << planetCnt << " fgM: " << fgMedia.size();

        // positioning
        switch (fgMedia.size()) {
            case 1:     (*fgMedia[0]).setDisplay(projectionW/2, projectionH/2, true);
        ofLogNotice("interaction") << "\t" << ofGetFrameNum() << "\t" << "positionRevolutions() position 1";
                        break;
            case 2:     (*fgMedia[0]).setDisplay(projectionW/2, projectionH/2 - 350, true);
                        (*fgMedia[1]).setDisplay(projectionW/2, projectionH/2 + 350, true);
        ofLogNotice("interaction") << "\t" << ofGetFrameNum() << "\t" << "positionRevolutions() position 2";
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
        ofLogNotice("interaction") << "\t" << ofGetFrameNum() << "\t" << "positionRevolutions() done";
    }

}

void planeApp::videoFollowBlob(int & blobID) {
    // find videoElement
    if (!transition && blobs[blobID].onStage) {
        ofPtr<mediaElement> vid = blobs[blobID].mediaLink;
        // update position
        ofPoint p;
        if (vid != NULL) {

            if (scene==4 && segment>3 && segment<6) {
                // PLANETS aligned on vertical, move up down
                p.y = blobMapToScreen(blobs[blobID].position).x * (16.0/9.0);
                p.x = offsetX + projectionW/2.0;
            } else if (scene==4 && segment<4) {
                // PLANETS aligned on horizontal
                p.y = projectionH/2;
                p.x = blobMapToScreen(blobs[blobID].position).x;
            } else {
                p = blobMapToScreen(blobs[blobID].position);
            }
            // set position
            if (!(*vid).moveElement) {
                (*vid).setDisplay( p, true);
            } else {
                (*vid).goal.set( p );
            }
            // set opacity
            if (scene==4 && segment==6) {
                float d = ofDist(p.x, p.y, projectionW/2, projectionH/2);
                (*vid).opMax = ((projectionW/2) - d) / (projectionW/2);
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

                    // update only if both blobs still exist!
                    std::map<int,Blob>::iterator iter1 = blobs.find(blobID);
                    std::map<int,Blob>::iterator iter2 = blobs.find(blob2ID);

                    // update only if bridge is not in self-destroy mode already
                    if (bridge != NULL && iter1 != blobs.end() && iter2 != blobs.end() && !(**it).selfdestroy) {
                        ofPoint p2 = blobMapToScreen(blobs[blob2ID].position);
                        float rot = -ofRadToDeg(atan2(p2.x-p.x,p2.y-p.y)) + 90;
                        float tx = bridgeX;
                        float ty = bridgeY;
                        float tz = sqrt( pow(tx,2) + pow(ty,2) );
                        float addRot = ofRadToDeg(atan2(tx,-ty));
                        float cx = p.x - tz*sin(ofDegToRad(rot+addRot));
                        float cy = p.y + tz*cos(ofDegToRad(rot+addRot));

                        (*bridge).setDisplay( cx, cy, false);
                        (*bridge).bridgeUpdated = true;;
                        (*bridge).w = ofDist(p.x, p.y, p2.x, p2.y) + tx*2;
                        (*bridge).rotation = rot;
                        ofLogVerbose("BRIDGE") << "\t" << ofGetFrameNum() << "\t" << "update bridge position " << blobID << "-" << blob2ID << "  to " << p.x << "|" << p.y << " w: "<< (*bridge).w;
                    } else {
                        if (bridge == NULL) {
                            ofLogVerbose("BRIDGE") << "\t" << ofGetFrameNum() << "\t" << "can't update bridge position, bridge==NULL " << blobID << "-" << blob2ID;
                        } else if (iter1 == blobs.end()) {
                            ofLogVerbose("BRIDGE") << "\t" << ofGetFrameNum() << "\t" << "can't update bridge position, iter1 doesn't exist " << blobID << "-" << blob2ID;
                        } else if (iter2 == blobs.end()) {
                            ofLogVerbose("BRIDGE") << "\t" << ofGetFrameNum() << "\t" << "can't update bridge position, iter2 doesn't exist " << blobID << "-" << blob2ID;
                        } else if ((**it).selfdestroy) {
                            ofLogVerbose("BRIDGE") << "\t" << ofGetFrameNum() << "\t" << "can't update bridge position, already selfdestroy " << blobID << "-" << blob2ID;
                        }
                    }
                }
            }
        }
    }
}

void planeApp::blobUnlink(int & blobID) {
    // making sure, a blob goes to die and untethers all video connections
    ofLogNotice("BLOB") << "\t\t" << ofGetFrameNum() << "\t" << "blobUnlink() \t" << blobID << "\tmediaLink: " << blobs[blobID].mediaLink;

    for (vector<ofPtr<mediaElement> >::iterator it = fgMedia.begin(); it != fgMedia.end(); it++) {
        if (*it == blobs[blobID].mediaLink) {
            ofPtr<mediaElement> vid = blobs[blobID].mediaLink;
            if (scene==1 && segment<2) {
                // STARS: fade out frozen stars
                (**it).fadeOut(0.001, 0.5, true);
                ofLogNotice("BLOB") << "\t\t" << ofGetFrameNum() << "\t" << "\t\t\tunlinked blob and fadeOut ";
            } else {
                (**it).dead = true;
                ofLogNotice("BLOB") << "\t\t" << ofGetFrameNum() << "\t" << "\t\t\tunlinked blob ";
            }
            blobs[blobID].mediaLink = ofPtr<mediaElement>();
            break;
        }
    }

    bridgeUnlink(blobID);
}

void planeApp::bridgeUnlink(int & blobID) {
    // find bridges for blobID, and send them to be unlinked
    for (vector<ofPtr<mediaElement> >::iterator it = fgMedia.begin(); it != fgMedia.end(); it++) {
        if ((**it).bridgeVideo && ((**it).bridgeBlobID[0]==blobID || (**it).bridgeBlobID[1]==blobID)) {
            Pair pair = Pair( min((**it).bridgeBlobID[0],(**it).bridgeBlobID[1]), max((**it).bridgeBlobID[0],(**it).bridgeBlobID[1]) );
            bridgeUnlink(pair);
        }
    }
}

void planeApp::bridgeUnlink(Pair & pair) {
    // delete bridge video if it exists
    bool found = false;
    for (vector<ofPtr<mediaElement> >::iterator it = fgMedia.begin(); it != fgMedia.end(); it++) {
        if ((**it).bridgeVideo && (**it).bridgeBlobID[0]==pair.blob1 && (**it).bridgeBlobID[1]==pair.blob2) {
            string videoFile = "video/2_stars/LINK_01-outro-photoJPEG.mov";
            if ((**it).file != videoFile) {
                ofLogNotice("BRIDGE") << "\t" << ofGetFrameNum() << "\tunlinked bridge\t" << (**it).bridgeBlobID[0] << " " << (**it).bridgeBlobID[1];
                int oldW = (**it).w;
                int oldH = (**it).h;
                (**it).loadMovie("video/2_stars/LINK_01-outro-photoJPEG.mov");
                (**it).w = oldW;
                (**it).h = oldH;
                (**it).reset(true);
                (**it).autoDestroy(true);
            }
        }
    }
    if (!found) ofLogNotice("BRIDGE") << "\t" << ofGetFrameNum() << "\t" << "\t\t\tbridge not found";
}

void planeApp::beginSegment() {
    endedSegment = false;
    ofLogNotice("TRANSITION") << "\t" << ofGetFrameNum() << "\t" << "beginSegment() " << blobs.size();

    transition = false;

    if (scene!=4) {
        // make new blob connections, to ensure blobs are connected to video elements, if necessary
        for(std::map<int, Blob>::iterator it = blobs.begin(); it != blobs.end(); ++it){
            Blob* b = &it->second;
            ofNotifyEvent(b->onCreate,b->id,b);
        }
    }

}

// trigger specific outro transformations for segment / scene 
//--------------------------------------------------------------
void planeApp::endSegment(int direction) {
    ofLogNotice("TRANSITION") << "\t" << ofGetFrameNum() << "\t" << "endSegment()   " << scene << ":" << segment;
    endedSegment = true;    // so that instructions are not displayed anymore
    segmentChange = direction;

    // ONLY DO BG-VIDEO TRANSITIONS IF A SCENE CHANGE IS COMING UP!
    if ((segment+segmentChange >= scenes[scene].segments || segment+segmentChange < 0) && scene!=3) {
        sceneChange = true;

        if (scene==1 && segment==3) {
            // STARS ENDING
            // make sure, all other fgMedia (planets) is gone already
            for (vector<ofPtr<mediaElement> >::iterator it = fgMedia.begin(); it != fgMedia.end(); ++it) {
                if (!(**it).dead) ((((**it)).*((**it)).outroTransformation))();
                else ofLogNotice("TRANSITION") << "\t" << ofGetFrameNum() << "\t\t\t" << "can't outro dead video: " << (**it).file;
            }

            fgMedia.push_back(ofPtr<mediaElement>( new videoElement("video/2_stars/ATTRACTION_outro-photoJPEG.mov")));
            (*fgMedia[fgMedia.size()-1]).setDisplay(projectionW/2,projectionH/2, true);
            (*fgMedia[fgMedia.size()-1]).reset();
            (*fgMedia[fgMedia.size()-1]).autoDestroy(true);
            (*fgMedia[fgMedia.size()-1]).movieEndTrigger=true;
            ofAddListener( (*fgMedia[fgMedia.size()-1]).fadeOutEnd, this, &planeApp::fgMediaFadedOut );

        } else if (scene==2 && segment==1) {
            // REVOLUTIONS OUTRO TRANSITION VIDEO
            // make sure, all other fgMedia (planets) is gone already
            for (vector<ofPtr<mediaElement> >::iterator it = fgMedia.begin(); it != fgMedia.end(); ++it) {
                if (!(**it).dead) ((((**it)).*((**it)).outroTransformation))();
            }
            // BIG SQUIGGLE
            // fgMedia.push_back(ofPtr<mediaElement>( new videoElement("video/3_revolution/REVOLUTION_outro-photoJPEG.mov")));
            // (*fgMedia[fgMedia.size()-1]).setDisplay(projectionW/2,projectionH/2, true);
            // (*fgMedia[fgMedia.size()-1]).reset();
            // (*fgMedia[fgMedia.size()-1]).autoDestroy(true);
            // (*fgMedia[fgMedia.size()-1]).movieEndTrigger=true;
            // ofAddListener( (*fgMedia[fgMedia.size()-1]).fadeOutEnd, this, &planeApp::bgMediaSwap );
            bgMediaSwap(scene);
            // moveOn = true;
        } else if (scene==4) {
            // fade out all planets
            int i=0;
            for (vector<ofPtr<mediaElement> >::iterator it = fgMedia.begin(); it != fgMedia.end(); ++it) {
                if (!(**it).dead) ((((**it)).*((**it)).outroTransformation))();
                if (i==0) ofAddListener( (**it).fadeOutEnd, this, &planeApp::allFaded );
                i++;
            }
        } else if (scene==5) {
            // dont' fade out all shooting stars
            moveOn = true;
        } else {
            int tmp = -1;
            fgMediaFadedOut(tmp);   // TODO clean up, just need to hand back ref. of integer
            // moveOn = true;
        }

        transition = true;
    } else {
        // if no BG fading is necessary, simply move on to the next segment
        if (scene==3 && (segment==2 || segment==4)) {
            if (success) {
                ofLogNotice("TRANSITION") << "\t" << ofGetFrameNum() << "\t" << "sunfreeze red";
                int randomFreezeSurface = ofRandom(4) + 1;
                //"SUN_freeze_3-qtPNG.mov";
                string newVideoName = "video/4_sun/SUN_freeze_" + ofToString(randomFreezeSurface) + "-qtPNG.mov";

                (*fgMedia[0]).loadMovie(newVideoName);
                (*fgMedia[0]).reset();

                // fgMedia.push_back(ofPtr<mediaElement>( new videoElement(newVideoName,false)));
                // (*fgMedia[fgMedia.size()-1]).setDisplay(projectionW/2,projectionH/2, true);
                // (*fgMedia[fgMedia.size()-1]).reset();
                if (segment==4) (*fgMedia[fgMedia.size()-1]).autoDestroy(true);
                else (*fgMedia[fgMedia.size()-1]).finishMovie(1.0);
                (*fgMedia[fgMedia.size()-1]).movieEndTrigger=true;
                ofAddListener( (*fgMedia[fgMedia.size()-1]).fadeOutEnd, this, &planeApp::bgMediaSwap );
                transition = true;
            } else {
                moveOn = true;
            }
            if(segment==4) sceneChange = true;
        } else if (scene==2 && segment==0) {
            // don't move to LET GO if there are no more planets on the screen
            if (planetCnt ==0) {
                ofLogNotice("TRANSITION") << "\t" << ofGetFrameNum() << "\t" << "endSegment()   planetCnt 0 ";
                segment+=2;
                sceneChange = true;
                bgMediaSwap(scene);
            } else {
                sceneChange = false;
                moveOn = true;
                ofLogNotice("TRANSITION") << "\t" << ofGetFrameNum() << "\t" << "endSegment()   planetCnt " << planetCnt;
            }
        } else {
            sceneChange = false;
            moveOn = true;
        }
    }
    ofLogNotice("TRANSITION") << "\t" << ofGetFrameNum() << "\t" << "endSegment()   sceneChange " << sceneChange;
}

void planeApp::jumpToScene(int s) {
    ofLogNotice("TRANSITION") << "\t" << ofGetFrameNum() << "\t" << "jump to scene " << s;
    int oldScene = scene;
    scene = s;
    segment = 0;
    sceneChange = true;
    transition = true;
    initSegment();
    int tmp = -2;
    if (scene==0) bgMediaSwap(scene);
    else if (oldScene==0 && scene>0) bgMediaSwap(tmp);
}

// decide what the next segment / scene will be
//--------------------------------------------------------------
void planeApp::nextSegment(int direction) {
    ofLogNotice("TRANSITION") << "\t" << ofGetFrameNum() << "\t" << "nextSegment " << direction;
    segment+=direction;

    // MOVE ON TO NEW SCENE
    if(scene == -1 || segment >= scenes[scene].segments) {
        scene++;
        segment = 0;
        if(scene >= scenes.size()) {
            language = (language==0) ? 1 : 0;
            if (blobsOnStage==0) scene = 0;
            else scene = 1;
        }

    } else if (segment < 0){
        scene--;
        if(scene < 0){
            scene = scenes.size()-1;
            language = (language==0) ? 1 : 0;
        }
        segment = scenes[scene].segments -1;
    }

    if (scene==2 && segment>1) {
        // skip KEEP DISTANCE if only 1 blob
        if (blobsOnStage<2) {
            scene++;
            segment = 0;
            sceneChange = true;
        }
    }

    if (lastActivity > inactivityTimer) {
        // no activity, got to idle mode
        ofLogNotice("TRANSITION") << "\t" << ofGetFrameNum() << "\t" << "no activity since " << lastActivity << "sec, go to IDLE";
        scene = 0;
        segment = 0;
        sceneChange = true;
    }

    if (scene==0) {
        bgMediaSwap(scene);
        globalStart = ofGetUnixTime();
    }

    if (sceneChange)
        ofLogNotice("TRANSITION") << "\t" << ofGetFrameNum() << "\t" << "sceneChange! scene " << scene << ": " << scenes[scene].name;
    ofLogNotice("TRANSITION") << "\t" << ofGetFrameNum() << "\t" << "---------------------------\t" << scene << " : " << segment;

    initSegment();
}

void planeApp::initSegment(){

    ofLogNotice("TRANSITION") << "\t" << ofGetFrameNum() << "\t" << "initSegment()";

    configureBlobserver();

    if (sceneChange) {
        ofLogNotice("TRANSITION") << "\t" << ofGetFrameNum() << "\t" << "cleared " << fgMedia.size() << " fgMedia videos";
        fgMedia.clear();   // delete all foreground videos
    }
    success = false;
    successCnt = 0;
    activityCnt = 0;
    followMe = -0.5;
    if (segment==0) planetCnt = 0;
    flash = true;       //
    segmentStart = ofGetUnixTime();

    // INSTRUCTION IMAGE, if present
    string in_img = scenes[scene].instructionImg[language][segment];
    if (in_img.length() > 2) {
        instructionImg.loadImage("img/" + in_img);
        // instructionImg.loadImage("img/placeholder_stop.jpg");
        ofLogNotice("TRANSITION") << "\t" << ofGetFrameNum() << "\t" << "load instruction image " << in_img;
    }

    if (scene==1 && segment<2) {
        (*bgMedia[5]).opMax = 0.35;
    } else (*bgMedia[5]).opMax = 1;

    // add FG videos
    if (scene==2) {
        // SPIN
        // if (sceneChange) {
        //     for (int i=0; i<5; i++) {
        //         string videoFile;
        //         int videoPick = i+1;
        //         videoFile = "video/3_revolution/REV_0"+ofToString(videoPick)+"-photoJPEG.mov";
        //         fgMedia.push_back(ofPtr<mediaElement>( new videoElement(videoFile,true)));
        //         (*fgMedia[i]).reset(true);
        //     }
        // } else {
        //     for (int i=0; i<5; i++) {
        //         if (segment==1 && i>=planetCnt) (*fgMedia[i]).dead = true;
        //     }
        // }
        // positionRevolutions();  // to position and turn on/off videos

    } else if (scene==3) {
        // SUN, load sun as fgMedia
        if (sceneChange) {

            fgMedia.push_back(ofPtr<mediaElement>( new videoElement("video/4_sun/SUN_stand_jump-loop_2-qtPNG.mov",false)));
            (*fgMedia[fgMedia.size()-1]).setDisplay(projectionW/2,projectionH/2, true);
            (*fgMedia[fgMedia.size()-1]).reset();
        } else if (segment==2) {

            (*fgMedia[0]).loadMovie("video/4_sun/SUN_run_loop-1-qtPNG.mov");
            (*fgMedia[0]).reset();
            // (*fgMedia[0]).dead = true;
            // fgMedia.push_back(ofPtr<mediaElement>( new videoElement("video/4_sun/SUN_run_loop-1-qtPNG.mov",false)));
            // (*fgMedia[fgMedia.size()-1]).setDisplay(projectionW/2,projectionH/2, true);
            // (*fgMedia[fgMedia.size()-1]).reset();
        } else if (segment==3) {
            // RUN EVERYWHERE
            (*fgMedia[0]).loadMovie("video/4_sun/SUN_run_loop-1-qtPNG.mov");
            (*fgMedia[0]).reset();
        }

    } else if (scene==4) {
        if (sceneChange) {
            // ECLIPSE. create white/black MAIN PLANET
            fgMedia.push_back(ofPtr<mediaElement>( new imageElement("video/5_eclipse/WHITE_PLANET.png")));
            (*fgMedia[fgMedia.size()-1]).setDisplay( projectionW/2, projectionH/2, true );
            (*fgMedia[fgMedia.size()-1]).reset();
            // (*fgMedia[fgMedia.size()-1]).opacity = 0.8;
            (*fgMedia[fgMedia.size()-1]).fadeTo(0.8);
        } else if (segment==2) {
            (*fgMedia[0]).setDisplay( projectionW/2, projectionH/2, true );
        } else if (segment==6) {
            // disperse
            (*fgMedia[0]).fadeOut(0.01, 1.0, true);
        }
    } else if (scene==5 && sceneChange) {
        // preload shooting stars
        for (int i=0; i<50; i++) {
            int randomShooter = ofRandom(26) + 1;
            if (randomShooter == 18 || randomShooter==26) randomShooter = 10;
            fgMedia.push_back(ofPtr<mediaElement>( new videoElement("video/6_shooting/SSTAR_" + ofToString(randomShooter,2,'0') + "-photoJPEG.mov")));
            (*fgMedia[fgMedia.size()-1]).setDisplay(-200,-500);
            (*fgMedia[fgMedia.size()-1]).play(false);
            (*fgMedia[fgMedia.size()-1]).reset();
        }
        shootingPointer = 0;
    }

}

// check for incoming OSC messages
// add new blobs, or update blobs with new data
//--------------------------------------------------------------
void planeApp::receiveOsc(){

	// check for waiting messages
	while(receiver.hasWaitingMessages()){
        if (!oscActive) oscActive =true;
        oscLastMsg = ofGetElapsedTimef();

		// get the next message
		ofxOscMessage m;
		receiver.getNextMessage(&m);

		if(m.getAddress() == "/blobserver/startFrame"){

            currentFlowId = m.getArgAsInt32(1);
            if (currentFlowId==hogFlowId) ofLogNotice("OSC") << "\t\t" << ofGetFrameNum() << "\t" << "/blobserver/startFrame " << m.getArgAsInt32(0);

            // tcout() << "start frame flow " << flowid << endl;
            if (currentFlowId==bgsubtractorFlowId) {
                bgsubtractorCnt = 0;
                bgsubtractorVel = 0.f;
                bgsubtractorAvVel = 0.f;
            }

        } else if(m.getAddress() == "/blobserver/endFrame"){
            oscMsgReceived = true;  // ready to process blob data
            currentFlowId = m.getArgAsInt32(1);
            if (currentFlowId==hogFlowId) ofLogNotice("OSC") << "\t\t" << ofGetFrameNum() << "\t" << "/blobserver/endFrame";
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
            ofLogNotice() << "BROADCAST" << var << ", " << source << ", " << cam << ", " << val;

		} else if(m.getAddress() == "/blobserver/hog" && currentFlowId==hogFlowId){
			// parse incoming elements:  iiiffiii: id x y vx vy age lost occluded
			int blobid = m.getArgAsInt32(0);
			int posx = m.getArgAsInt32(1);
			int posy = m.getArgAsInt32(2);
			float velx = m.getArgAsFloat(3);
			float vely = m.getArgAsFloat(4);
			int age = m.getArgAsInt32(5);
            int lost = m.getArgAsInt32(6);
			int occluded = m.getArgAsInt32(7);
            ofLogNotice("OSC") << "\t\t" << ofGetFrameNum() << "\t" << "/blobserver/hog\tid: " << blobid << "\t lost:" << lost;

			std::map<int,Blob>::iterator iter = blobs.find(blobid);
            bool newBlob = false;
			if( iter != blobs.end() ) {
                // blob instance already exists
			} else {
                // create new blob instance
                newBlob = true;
				blobs[blobid].id = blobid;
                ofLogNotice("BLOB") << "\t\t" << ofGetFrameNum() << "\t" << "receiveOsc()\tNew Blob\t" << blobid;
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
                // ofAddListener( blobs[blobid].onSteadyReward, this, &planeApp::blobSteadyReward );
                ofAddListener( blobs[blobid].onBreakSteady, this, &planeApp::blobBreakSteady );
                ofAddListener( blobs[blobid].onEnterStage, this, &planeApp::blobEnterStage );
                ofAddListener( blobs[blobid].onLeaveStage, this, &planeApp::blobLeaveStage );
            }
            // update blob with new values
            Blob* b = &blobs.find(blobid)->second;
            b->follow(posx + blobW/2.0, posy + blobH*0.8, siteW, siteH, stageRadius, y_mean);
            b->setVelocity(velx, vely);
            // b->analyze(freezeMaxVel, freezeMinTime, freezeMaxTime, movingThr);    //
            b->age = age;
            b->lostDuration = lost;
            b->occluded = (occluded==1) ? true : false;
            if (newBlob) ofNotifyEvent( blobs[blobid].onCreate, blobid, &blobs[blobid] );
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
        int offsx = 500;
        if(!projectorOn) offsx = 10;
        int offsy = 10;

        ofFill(); ofSetColor(255);
        ofDrawBitmapString(ofToString(ofGetFrameRate()), offsx, offsy);

        offsy += 20;

        this->drawRawData(offsx, offsy, 0.5);

        offsy += 260 + 10;
        this->drawTopDown(offsx, offsy, 0.5, drawBlobDetail);

        offsy = 10;
        offsx += 260;
        this->drawAnalysis(offsx, offsy, 0.35);

        offsy = 10;
        offsx += 390;
        this->drawScreen(offsx, offsy, 0.35);

        offsx += 440;
        this->drawControlInfo(offsx, offsy);

        gui.draw();

        // MAIN VISUALS SCREEN
        ofPushMatrix();
        ofTranslate(0,2100);
        ofRotateZ(-90);
        if (testMode) {
            // this->drawRawData(1490, 50, 2);
            // this->drawTopDown(1490, 850, 2, drawBlobDetail);
            this->drawRawData(0, 50, 2);
            this->drawTopDown(0, 850, 2, drawBlobDetail);
        } else this->drawScreen(0, 0, 1);

        ofFill(); ofSetColor(255);
        ofDrawBitmapString(ofToString(ofGetFrameRate()), 10, 10);
        ofPopMatrix();

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


    // BACKGROUND VIDEOS
    ofEnableBlendMode(OF_BLENDMODE_ADD);
    nebula->draw(x+projectionOffsetX*scale,y,scale);
    (*bgMedia[bgMediaId]).draw(x+projectionOffsetX*scale,y,scale);
    ofDisableBlendMode();


    // FOREGROUND VIDEOS

    // foreground videos, with BLENDING
    ofEnableBlendMode(OF_BLENDMODE_ADD);
    for (vector<ofPtr<mediaElement> >::iterator it = fgMedia.begin(); it != fgMedia.end(); ++it) {
        if (!(**it).dead && (**it).blend) (**it).draw(x, y, scale);   //
    }
    ofDisableBlendMode();

    // foreground videos, without BLENDING mode
    ofEnableAlphaBlending();
    for (vector<ofPtr<mediaElement> >::iterator it = fgMedia.begin(); it != fgMedia.end(); ++it) {
        if (!(**it).dead && !(**it).blend) (**it).draw(x, y, scale);
    }
    ofDisableAlphaBlending();



    // INSTRUCTIONS
    string in_img = scenes[scene].instructionImg[language][segment];

    // INSTRUCTION TEXT
    if (!transition && !endedSegment && segmentClock>0) {
        int ypos = 1742;
        int textLineH = 40;
        ofFill();

        string instruction = scenes[scene].instructions[language][segment];
        string measureInst = instruction;
        // hack to allow for french-accented letters to produce correct center-align text-block width
        if(language==1) measureInst = scenes[scene].instructions[2][segment];

        if (scene>0) {


            ofSetColor(255);
            // multiple lines, need to split up, to center-align
            vector< string > instructionLine = ofSplitString(instruction, "\n");
            vector< string > measureInstLine = ofSplitString(measureInst, "\n");

            int i = 0;
            for (vector<string>::iterator it = instructionLine.begin() ; it != instructionLine.end(); ++it) {

                ofRectangle textR = fontBg.getStringBoundingBox(measureInstLine[i],0, 0);
                if (scene==4 && segment==1) {
                    // FOLLOW ME
                    fontBg.drawString((*it), x+ ((*fgMedia[0]).position.x - textR.width/2)*scale, y+ypos*scale);
                } else {
                    fontBg.drawString((*it), x+ (projectionW/2 - textR.width/2)*scale, y+ypos*scale);
                }
                ypos += textLineH;
                i++;
            }
        } else {
            // IDLE MODE, one liner therefore no need to split

            int animCnt = ofGetFrameNum()%120;
            float dimAlpha = 0.3;
            if (animCnt < 30) {
                ofSetColor(255, int (255*dimAlpha + (animCnt/30.f)*(255*(1-dimAlpha))));
            } else if (animCnt > 90) {
                ofSetColor(255, int (255 - ((animCnt-90)/30.f)*(255*(1-dimAlpha))));
            } else {
                ofSetColor(255);
            }
            ofRectangle textR = fontBg.getStringBoundingBox(measureInst,0,0);
            fontIdle.drawString(instruction, x+ (projectionW/2 - textR.width/2)*scale, y+ypos*scale);


            // ofPushMatrix();
            // ofTranslate(x+ (projectionW/2 - textR.width/2)*scale, y+ypos*scale);
            // ofScale(scale, scale, scale);
            // fontIdle.drawString(instruction, 0, 0);
            // ofPopMatrix();
        }
    }

    // INSTRUCTION IMAGES, display always, so that they are visible on transition videos (red sun)
    if (in_img.length() > 2) {
        ofEnableBlendMode(OF_BLENDMODE_ADD);
        ofFill();
        ofSetColor(255,255,255);
        instructionImg.draw(x,y, projectionW*scale, projectionH*scale);
        ofDisableBlendMode();
    }

    // extra debug information
    if (displayDebug) {
        fontSm.drawString(scenes[scene].analysis[segment], x+100*scale, y+(projectionH-150)*scale);
        fontBg.drawString(ofToString(segmentClock), x+(projectionW-200)*scale, y+(projectionH-200)*scale);
        fontSm.drawString(ofToString(success ? "true" : "false"), x+(projectionW-200)*scale, y+(projectionH-150)*scale);
    }

    if (flash) {
        ofEnableAlphaBlending();
        float alpha = (flashCnt < flashMax/2) ? (flashCnt/(flashMax/2.f)) : (flashMax - flashCnt)/(flashMax/2.f);
        ofSetColor(flashColor.get().r,flashColor.get().g,flashColor.get().b,flashColor.get().a*alpha); ofFill();
        ofRect(x,y,projectionW*scale,projectionH*scale);
        ofDisableAlphaBlending();
    }
}

//--------------------------------------------------------------
void planeApp::drawAnalysis(int x, int y, float scale){

    // frame first
    ofNoFill(); ofSetColor(255);
    ofRect(x,y,1000*scale,projectionH*scale);

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
                    textStr += "\nvel \t\t: "+ ofToString(b->vel,2);
                    ofDrawBitmapStringHighlight(textStr, bx+70, by -40);

                    for(std::map<int, Neighbor>::iterator iter = b->neighbors.begin(); iter != b->neighbors.end(); ++iter){
                        Neighbor* nb = &iter->second;
                        if (nb->steadyDistance) ofSetColor(255); else ofSetColor(0);
                        ofDrawBitmapString(ofToString(nb->id) + " stdDev "+ ofToString(nb->getStdDev(),0), bx+70, by+15);
                        ofRect(bx+180, by+15, nb->distance[0]*0.2, -10);
                        by += 15;
                    }

                    by += 60;
                }
            }
        }
    } else if (scene==3 || scene==4) {

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

                string textStr = "lost\t: "+ ofToString(b->lostDuration);
                textStr += "\noccluded: "+ ofToString(b->occluded ? "true" : "false");
                textStr += "\nvel \t: "+ ofToString(b->vel,2);
                ofDrawBitmapStringHighlight(textStr, bx+70, by -20);

                by += 70;
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

                string textStr = "lost\t: "+ ofToString(b->lostDuration);
                textStr += "\noccluded: "+ ofToString(b->occluded ? "true" : "false");
                textStr += "\nvel \t: "+ ofToString(b->vel);
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
                       "\nLAST ACTIVITY\t" + ofToString(lastActivity) +
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
    string rawInfo = "port: \t\t" + ofToString(MYPORT);
    rawInfo += "\nBLOBSERVER: \t" + ofToString(BLOBSERVERIP); //  + " (" + ofToString(BLOBPORT) + ")";
    rawInfo += "\nosc active: \t" + ofToString(oscActive ? "true" : "false");
    rawInfo += "\nosc last msg: \t" + ofToString(oscLastMsgTimer,2) + " sec";
    rawInfo += "\nexposure: \t" + ofToString(cameraExposure,5);
    rawInfo += "\nbgs blob cnt: \t" + ofToString(bgsubtractorCnt);
    rawInfo += "\nbgs avg vel: \t" + ofToString(bgsubtractorAvVel,2);
    rawInfo += "\nhog velocity: \t" + ofToString(hogAvVel,2);
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
            if ((*it).point.x > 0 && (*it).point.y > 0) {
                ofVertex( x + (*it).point.x*scale, y + (*it).point.y*scale );
            }
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
    if (blobsOnStage<=0)
        return false;

    bool allAligned = true;
    for(std::map<int, Blob>::iterator it = blobs.begin(); it != blobs.end(); ++it){
        Blob* b = &it->second;
        if (b->onStage) {
            float x = blobMapToScreen(b->position).x;
            // float y = offsetY + b->position.y * mapSiteH;
            float d = ofDist(x, 0, p.x, 0);
            if (d > alignmentMaxDist) {
                allAligned = false;
                break;
            }
        }
    }
    return allAligned;
}


ofPoint planeApp::blobMapToScreen(ofPoint &o) {
    ofPoint p;
    p.x = projectionW - (offsetX + o.x * mapSiteW);
    // p.y = offsetY + o.y * mapSiteH;
    p.y = projectionH - (offsetY + o.y * mapSiteW);
    return p;
}




void planeApp::configureBlobserver() {

    if (configBlobserver && 1==2) {

        ofLogNotice("TRANSITION") << "\t" << ofGetFrameNum() << "\t" << "configureBlobserver()";

        // standard setting:
        bool erratic = false;  // else slow movements
        bool allowLessFP = false;        // allow more false positives
        bool slow = false;              // extra slow?

        if (scene==0) {                 // IDLE

        } else if (scene==1) {          // STARS
            slow = true;

            if (segment==0) {               // STAND STILL
                slow = true;
            } else if (segment==1) {        // STAND STILL SOME MORE
                slow = true;
            } else if (segment==2) {        // KEEP THE DISTANCE
            } else if (segment==3) {        // KEEP THE DISTANCE P2
            }

        } else if (scene==2) {          // REVOLUTION

            if (segment==0) {               // SPIN
                erratic = true;
            } else if (segment==1) {        // LET GO
                erratic = true;
            }

        } else if (scene==3) {          // SUN

            if (segment==0) {               // STAND ON ONE LEG
            } else if (segment==1) {        // HOP
                erratic = true;
                allowLessFP = true;
            } else if (segment==2) {        // FREEZE
            } else if (segment==3) {        // RUN EVERYWHERE
            } else if (segment==4) {        // FREEZE
            }

        } else if (scene==4) {          // ALIGNMENT

            slow = true;

            if (segment==0) {               // LINE UP IN FRONT OF ME
            } else if (segment==1) {        // FOLLOW ME
            } else if (segment==2) {        // STEP OUT OF THE LINE
            } else if (segment==3) {        // STEP INTO THE LINE
            } else if (segment==4) {        // STEP OUT OF THE LINE
            } else if (segment==5) {        // STEP INTO THE LINE
            } else if (segment==6) {        // DISPERSE SLOWLY
            }

        } else if (scene==5) {          // SHOOTING STARS
            erratic = true;
            allowLessFP = true;
            if (segment==0) {               // MOVE LIKE A SHOOTING STAR
            } else if (segment==1) {        // DROP
            }

        }

        if (slow) {
            //
            sendOscMsgToHog("setParameter", "processNoiseCov", pow(10, -noiseSlow));
            sendOscMsgToHog("setParameter", "measurementNoiseCov", pow(10, -measurementSlow));
        } else if (erratic) {
            // ERRATIC FAST MOVEMENTS, allow for jumping, running, etc.
            sendOscMsgToHog("setParameter", "processNoiseCov", pow(10, -noiseErratic));
            sendOscMsgToHog("setParameter", "measurementNoiseCov", pow(10, -measurementErratic));
        } else {
            // NORMAL SETTING : SLOW MOVEMENTS, don't allow for erratic movements
            sendOscMsgToHog("setParameter", "processNoiseCov", pow(10, -noiseNormal));
            sendOscMsgToHog("setParameter", "measurementNoiseCov", pow(10, -measurementNormal));
        }

        if (allowLessFP) {
            // ALLOW FOR LESS FALSE POSITIVES (jumping)
            // sendOscMsgToHog("setParameter", "margin", 0.3);
        } else {
            // NORMAL SETTING : MARGIN
            sendOscMsgToHog("setParameter", "margin", 0.0);
        }
    }

}


void planeApp::exit() {
    // do some destructing here
    // sendOscMsg("signOut", MYIP); // don't send disconnect, else blobserver terminates
    ofLogNotice() << "goodbye";
}


//--------------------------------------------------------------
void planeApp::keyPressed(int key){

}

void planeApp::guiNebulaChange(int & v) {
    ofLogNotice() << "\t\t" << ofGetFrameNum() << "\t" << "guiNebulaChange() " << v;
    nebula->opMax = nebulaOpacity/100.f;
}

    // try {
    // } catch(exception& e){
    //     ofLogError() << e.what();
    // }


void planeApp::printDebugInfo() {
    // print blob data
    ofLogError();
    ofLogError() << "-----------------------------------------------";
    ofLogError();
    for(std::map<int, Blob>::iterator it = blobs.begin(); it != blobs.end(); ++it){
        Blob* b = &it->second;
        ofLogError() << "BLOB\t" << b->id << "\tpos: " << ofToString(b->position.x,0) << " | " << ofToString(b->position.y,0) << "\tonStage: " << b->onStage << "\tvel: " << ofToString(b->velocity.x,0) << " | " << ofToString(b->velocity.y,0);
        ofLogError() << "\t" << "\tage: " << ofToString(b->age,0) << "\tlost: " << ofToString(b->lostDuration,0);
        ofLogError() << "\t" << "\tmediaLink: " << b->mediaLink;
    }
    ofLogError();
    for (vector<ofPtr<mediaElement> >::iterator it = fgMedia.begin(); it != fgMedia.end(); it++) {
        ofPtr<mediaElement> v = (*it);
        ofLogError() << "VIDEO\t\t" << v << "\t" << v->file;
        ofLogError() << "\t" << "\tbridge: " << v->bridgeVideo << "\t" << v->bridgeBlobID[0] << "-" << v->bridgeBlobID[1];
        ofLogError() << "\t" << "\tpos: " << ofToString(v->position.x,0) << " | " << ofToString(v->position.y,0);
    }
}


void planeApp::sendOscMsg(string addr, string v1, int v2) {
    ofxOscMessage m;
    m.setAddress( "/blobserver/"+addr );
    m.addStringArg(v1);
    m.addIntArg(v2);
    sender.sendMessage( m );
    ofLogNotice("OSC") << "\t\t" << ofGetFrameNum() << "\t" << "send OSC: /blobserver/" << addr << " " << v1 << " " << v2;
    m.clear();
}



void planeApp::sendOscMsgToHog(string addr, string v1, float v2) {
    ofxOscMessage m;
    m.setAddress( "/blobserver/"+addr );
    m.addStringArg(MYIP);
    m.addIntArg(hogFlowId);
    m.addStringArg(hogFlowName);
    m.addStringArg(v1);
    m.addFloatArg(v2);
    sender.sendMessage( m );
    ofLogNotice("OSC") << "\t\t" << ofGetFrameNum() << "\t" << "send OSC: /blobserver/" << addr << " " << v1 << " " << v2;
    m.clear();
}


//--------------------------------------------------------------
void planeApp::keyReleased(int key){

    if (key == 'd') {
        // drawBlobDetail = !drawBlobDetail;
        cout << "print debug info" << endl;
        printDebugInfo();
    }

    if (key == 'b') {
        drawBridge = !drawBridge;
    }

    if (key == OF_KEY_RETURN) {
        processing = !processing;
    }

    if (key == OF_KEY_RIGHT){
        if (!transition) {
            endSegment(1);
            ofLogNotice("KEY") << "\t\t\t" << ofGetFrameNum() << "\t" << "==============>" << key << " end segment ";
        }
    }
    if (key == ' ') {
        autoplay = !autoplay;
    }
    if (key == 't') {
        testMode = !testMode;
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
        ofLogNotice() << "make screenshot";
        ofImage img;
        img.grabScreen(0,0,1425,700);
        string fileName = "plane_"+ofGetTimestampString()+".png";
        img.saveImage(fileName);
        ofLogNotice() << "saved screenshot " << fileName.c_str();
    }

    if (key>='0' && key<='5') {
        int s = int(key) - int('0');
        ofLogNotice("KEY") << "\t\t\t" << ofGetFrameNum() << "\t" << "==============>" << key << " scene = " << s;
        jumpToScene(s);
    }

    if (key=='r' && scene==5) {
        // shooting star
        if (++shootingPointer>=fgMedia.size()) shootingPointer=0;
        // (*fgMedia[shootingPointer]).setDisplay(ofRandom(projectionW-100), ofRandom(projectionH-100), true);
        (*fgMedia[shootingPointer]).moveAcross( ofRandom(-2.0,2.0), ofRandom(-2.0,2.0), projectionW, projectionH, false);
        (*fgMedia[shootingPointer]).reset();
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
