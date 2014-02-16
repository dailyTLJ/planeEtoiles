#include "planeApp.h"
#include "ofBitmapFont.h"


std::ostream& tcout() {
  // Todo: get a timestamp in the desired format
  return std::cout << ofGetFrameNum() << " \t";
}

//--------------------------------------------------------------
void planeApp::setup(){

    ofSetFrameRate(30);
    
    ofSetLogLevel(OF_LOG_NOTICE);

    // create a new logfile everytime the application is started
    if (ofGetWindowMode() == OF_FULLSCREEN) 
        ofLogToFile("/home/planeviz/Documents/log/of_log/planeEtoiles_"+ofGetTimestampString()+".log");
    ofLogNotice("START") << "\t" << ofGetFrameNum() << "\t" << "setup";

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

    ofSetLogLevel("BLOB", OF_LOG_NOTICE);
    ofSetLogLevel("KEY", OF_LOG_NOTICE);
    ofSetLogLevel("BRIDGE", OF_LOG_NOTICE);
    ofSetLogLevel("TRANSITION", OF_LOG_NOTICE);
    ofSetLogLevel("OSC", OF_LOG_WARNING);
    ofSetLogLevel("interaction", OF_LOG_NOTICE);
    ofSetLogLevel("videoElement", OF_LOG_NOTICE);
    ofSetLogLevel("mediaElement", OF_LOG_NOTICE);



	ofTrueTypeFont::setGlobalDpi(72);

    // font to draw text instructions
    fontBg.loadFont("CircularStd-Book.otf", 34, true, true);
    fontBg.setLineHeight(40.0f);
    fontBg.setLetterSpacing(1.037);
    fontBg.setSpaceSize(0.5);

    // slightly bigger font to draw 'come closer' instruction, in idle scene
    fontIdle.loadFont("CircularStd-Book.otf", 46, true, true);
    fontIdle.setLineHeight(56.0f);
    fontIdle.setLetterSpacing(1.037);
    fontIdle.setSpaceSize(0.5);

    // font only used for GUI, debug display
    fontSm.loadFont("Mercury Bold.otf", 20, true, true);
	fontSm.setLineHeight(16.0f);
	fontSm.setLetterSpacing(1.037);

    projectorOn = (ofGetWindowMode() == OF_FULLSCREEN) ? true : false;
    // projectorOn = false;
    processing = true;
    oscMsgReceived = false;
    oscLastMsg = 0;
    oscLastMsgTimer = 0;
    oscLastMsgTimerMax = 10;
    oscNetworkReady = false;
    oscActive = false;
	mouseX = 0;
	mouseY = 0;
	mouseButtonState = "";
    lastActivityClock = ofGetUnixTime();
    lastActivity = 0;
    moonPosX = 40;

    success = false;
    successCnt = 0;
    blobsOnStage = 0;
    activityCnt = 0;
	drawBlobDetail = false;
    transition = false;
    drawDebugScreen = true;
    moveOn = false;
    sceneChange = false;
    flash = false;
    flashCnt = 0;
    flashMax = 2;
    bgMediaId = 5;
    pickPlanet = 0;

    drawBridge = false;
    bridgeX = 5;
    bridgeY = 55;

    projectionW = 1200;
    projectionH = 1920;
    projectionOffsetX = 60;
    blobserverW = 564;
    blobserverH = 320;
    blobW = 80;
    blobH = 160;

    hogFlowId = 1;
    hogFlowName = "Actuator";
    hogAvVel = 0.f;

    followMe = -0.5;
    shootingPointer = 0;

    siteW.addListener(this,&planeApp::recalculatePerspective);
	siteH.addListener(this,&planeApp::recalculatePerspective);

    // reposition GUI based on number of screens
    int transX = (!projectorOn) ? 0 : 500;

    stateGui.setup("CURRENT STATE", "currentState.xml", 10 + transX, 580);
    stateGui.add(guiScene.set("Current Scene", 1, 0, 6));
    stateGui.add(guiSegment.set("Current Segment", 0, 0, 8));
    stateGui.add(language.set("Current Language (EN/FR)", 1, 0, 1));
    stateGui.add(languageCnt.set("Language Counter", 0, 0, 5));
    stateGui.add(languageRatio.set("French_English_Ratio", 1, 1, 5));
    stateGui.setSize(220,130);
    stateGui.loadFromFile("currentState.xml");

    gui.setup("HUMANS AND PLANETS", "planets01.xml", 1204 + transX,10);
    gui.setDefaultBackgroundColor( ofColor(0,0,50) );
    gui.add(autoplay.setup("autoplay", false));
    // gui.add(testMode.setup("testMode", false));
    gui.add(displayDebug.setup("displayDebug", false));
    gui.add(minLostTime.set("Min LostTime", 1, 0, 10));
    gui.add(inactivityTimer.set("inactivity timer", 30, 10, 200));
    gui.add(minSegmentLength.set("Min Segm Length", 3, 0, 10));
    gui.add(nebulaOpacity.set("Nebula Opacity", 50, 0, 100));
    gui.add(instructionFadeIn.set("Instruct Fade In",0.1, 0.001, 0.4));
    gui.add(instructionFadeOut.set("Instruct Fade Out",0.1, 0.001, 0.4));
    // gui.add(flashColor.set("Transition Flash Color",ofColor(255,200,100),ofColor(0,0),ofColor(255,255)));

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
    gui.add(paramSc1);

    paramSc6.setName("Sc2 Attraction");
    paramSc6.add(distStdDevThr.set( "Dist StdDev", 10, 0, 70));
    paramSc6.add(movingThr.set( "Movement Thr", 0.1, 0, 3));
    // paramSc6.add(steadyRewardTime.set( "Dist Reward", 2, 0, 10));
    gui.add(paramSc6);

    paramSc2.setName("Sc3 Revolutions");
    paramSc2.add(minLostSpin.set("MinLost SPIN", 1, 0, 10));
    paramSc2.add(spinJudgeTime.set( "spinJudgeTime", 180, 0, 240));
    paramSc2.add(spinSuccess.set( "spinSuccess", 5, 0, 10));
    paramSc2.add(spinFailure.set( "spinFailure", 2, 0, 10));
    gui.add(paramSc2);

    paramSc3.setName("Sc4 Sun");
    // paramSc3.add(edgeMargin.set( "Edge Margin", 50, 0, 150));
    paramSc3.add(minLostHop.set("MinLost HOP", 1, 0, 10));
    // paramSc3.add(activityColorCh.set( "Activity Color Change", 10, 0, 30));
    paramSc3.add(freezeAllMaxVel.set( "FreezeAllMaxVel",0.1, 0, 3.0 ));
    // paramSc3.add(freezeJudgeTime.set( "freezeJudgeTime", 180, 0, 240));
    // paramSc3.add(freezeVideoSpeedMap.set( "freezeVideoSpeedMap", 0.1, 0, 1));
    paramSc3.add(runJudgeTime.set( "runJudgeTime", 80, 0, 240));
    paramSc3.add(runHogThr.set( "runHogThr", 1.0, 0, 10));
    // paramSc3.add(runBgsThr.set( "runBgsThr", 3.0, 0, 10));
    paramSc3.add(runActThr.set( "runActThr", 5, 0, 20));
    gui.add(paramSc3);

    paramSc4.setName("Sc5 Eclipse");
    paramSc4.add(alignmentMaxDist.set( "Alignm MaxDist", 50, 0, 200));
    paramSc4.add(alignmentTransition.set( "Alignm Transition", 10, 0, 20));
    paramSc4.add(followMeSpeed.set( "followMeSpeed", 0.002, 0, 0.02));
    paramSc4.add(followMeRadius.set( "followMeRadius", 300, 0, 500));
    gui.add(paramSc4);

    paramSc5.setName("Sc6 Combustion");
    paramSc5.add(minLostShoot.set("MinLost SHOOT", 1, 0, 10));
    gui.add(paramSc5);

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
        oscNetworkReady = true;
    } catch(exception& e){
        ofLogError() << e.what();
    }


    instructionTxt.setText("test", "test");
    instructionTxt.opacity = 0.0;
    // instructionImg.loadImage("img/placeholder_letgo.jpg");

    // load instruction-video placeholder
    instructionVid = ofPtr<mediaElement>( new videoElement());
    instructionVid->setDisplay(projectionW/2,projectionH/2, true);
    ofAddListener( instructionVid->playLoop, this, &planeApp::placeInstruction );

    // compute the perspectiveTransformation
    // to map from blob-coordinates to the top-down view
    this->setPerspective();

    int cx = 75;
    int cy = 50;
    steles[0].set(313-cx,203-cy,0);
    steles[1].set(492-cx,208-cy,0);
    steles[2].set(615-cx,237-cy,0);
    steles[3].set(567-cx,293-cy,0);
    steles[4].set(341-cx,314-cy,0);
    steles[5].set(150-cx,293-cy,0);
    steles[6].set(179-cx,237-cy,0);

    int tmp = 1;
    recalculatePerspective(tmp);

    // init
    this->initScenes();

	ofBackground(255);

}

void planeApp::recalculatePerspective(int & v) {
    ofLogNotice() << "recalculatePerspective\t" << "siteW " << siteW << "\tsiteH " << siteH;
    this->setPerspective();

    for (int i=0; i<7; i++) {
        vector<cv::Point2f> pre, post;
        pre.push_back(cv::Point2f(steles[i].x, steles[i].y));
        cv::perspectiveTransform(pre, post, perspectiveMat);
        steles_topdown[i].set(post[0].x, post[0].y,0);
    }
}

void planeApp::setPerspective() {
    int skew = 0;
    int cx = 75;
    int cy = 50;
    // float in[] = {260-78,140-16,600-78,210-16,380-78,300-16,-150-78,300-16};
    float in[] = { 310-cx,178-cy,    750-cx,223-cy,     540-cx,353-cy,    -150-cx,293-cy};
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
	segmentClock = 0;
	masterClock = 0;
    resetClock = false;

    string sampleVideoFile = "video/3_revolution/REV_01-photoJPEG.mov";
    sampleVideoFile = "video/REV_01-animation.mov";
    sampleVideoFile = "video/SUN_explosion-11-animation.mov";


    // ******************* preload videos ********************* //

    // nebula = ofPtr<mediaElement>( new videoElement("video/NEBULA_131211-H264.mp4"));
    nebula = ofPtr<mediaElement>( new videoElement("video/NEBULA_140130_animation_3fps.mov"));
    nebula->reset(true);
    nebula->opMax = nebulaOpacity/100.f;
    nebulaOpacity.addListener(this,&planeApp::guiNebulaChange);

    bgMedia.push_back(ofPtr<mediaElement>( new videoElement("video/IDLE_MODE_11-half-1-H264-10mbps.mp4",false)));       // 0
    bgMedia.push_back(ofPtr<mediaElement>( new videoElement("video/IDLE_MODE_13_blue-anim-H264-10mbps.mp4",false)));    // 1
    bgMedia.push_back(ofPtr<mediaElement>( new videoElement("video/IDLE_MODE_13_white-anim_animation_15fps.mov",false)));   // 2
    bgMedia.push_back(ofPtr<mediaElement>( new videoElement("video/IDLE_MODE_13_yellow-anim-H264-10mbps.mp4",false)));  // 3
    bgMedia.push_back(ofPtr<mediaElement>( new videoElement("video/BACKGROUND-1_intro-H264_10mpbs.mp4",false)));        // 4
    (*bgMedia[4]).opMax = 0.35;
    bgMedia.push_back(ofPtr<mediaElement>( new videoElement("video/BACKGROUND-1_loop-H264_10mpbs.mp4",false)));         // 5
    bgMedia.push_back(ofPtr<mediaElement>( new videoElement("video/4_sun/SUN_26-intro-photoJPEG.mov",false)));          // 6

    attraction_outro = ofPtr<mediaElement>( new videoElement("video/2_stars/ATTRACTION_outro-photoJPEG.mov"));
    sun_jump = ofPtr<mediaElement>( new videoElement("video/4_sun/SUN_stand_jump-loop_4-qtPNG.mov",false));
    sun_run = ofPtr<mediaElement>( new videoElement("video/4_sun/SUN_run_loop-4-qtPNG.mov",false));
    sun_freeze_red.push_back( ofPtr<mediaElement>( new videoElement("video/4_sun/SUN_freeze_1-qtPNG.mov",false)));


    // PRELOAD BLUE SURFACE VIDS = seems to slow down FPS
    // for (int i=1; i<13; i++) {
    //     sun_surface_blue.push_back( ofPtr<mediaElement>(
    //         new videoElement("video/4_sun/SUN_run_surface-"+ofToString(i)+"-blue-photoJPEG.mov",false)));
    // }

    // PRELOAD PLANET VIDS = seems to slow down FPS
    // int planedId[] = { 6, 9, 13, 15, 18, 19,20, 22, 23 };
    // planet_animated.push_back( ofPtr<mediaElement>( new videoElement("video/5_eclipse/P_6-qtPNG.mov")));
    // planet_animated.push_back( ofPtr<mediaElement>( new videoElement("video/5_eclipse/P_9-qtPNG.mov")));
    // planet_animated.push_back( ofPtr<mediaElement>( new videoElement("video/5_eclipse/P_13-qtPNG.mov")));
    // planet_animated.push_back( ofPtr<mediaElement>( new videoElement("video/5_eclipse/P_15-qtPNG.mov")));
    // planet_animated.push_back( ofPtr<mediaElement>( new videoElement("video/5_eclipse/P_18-qtPNG.mov")));
    // planet_animated.push_back( ofPtr<mediaElement>( new videoElement("video/5_eclipse/P_19-qtPNG.mov")));
    // planet_animated.push_back( ofPtr<mediaElement>( new videoElement("video/5_eclipse/P_20-qtPNG.mov")));
    // planet_animated.push_back( ofPtr<mediaElement>( new videoElement("video/5_eclipse/P_22-qtPNG.mov")));
    // planet_animated.push_back( ofPtr<mediaElement>( new videoElement("video/5_eclipse/P_23-qtPNG.mov")));

    for (int i=0; i<75; i++) {
        int randomShooter = ofRandom(26) + 1;
        if (randomShooter == 18 || randomShooter==26) randomShooter = 10;
        shooting_stars.push_back(ofPtr<mediaElement>( new videoElement("video/6_shooting/SSTAR_" + ofToString(randomShooter,2,'0') + "-photoJPEG.mov")));
    }

    title_sequence.push_back(ofPtr<mediaElement>( new videoElement("video/TITLE_02-EN-photoJPEG.mov")));
    title_sequence.push_back(ofPtr<mediaElement>( new videoElement("video/TITLE_02-FR-photoJPEG.mov")));
    diagram_sequence.push_back(ofPtr<mediaElement>( new videoElement("video/DIAGRAM_01-EN-photoJPEG.mov")));
    diagram_sequence.push_back(ofPtr<mediaElement>( new videoElement("video/DIAGRAM_01-FR-photoJPEG.mov")));
    // title_sequence.push_back(ofPtr<mediaElement>( new videoElement(sampleVideoFile)));
    // title_sequence.push_back(ofPtr<mediaElement>( new videoElement(sampleVideoFile)));
    // diagram_sequence.push_back(ofPtr<mediaElement>( new videoElement(sampleVideoFile)));
    // diagram_sequence.push_back(ofPtr<mediaElement>( new videoElement(sampleVideoFile)));


    int s = 0;

    // ****************** define scene information ****************** //

    sceneInfo idle;
    idle.name = "Idle";
    idle.no = n;
    idle.segments = 1;
    idle.analysis[0] = "* detection";
    idle.instructions[0][0] = "Come closer\nStep inside the dancing zone";
    idle.instructions[1][0] = "Approchez\nEntrez dans la zone de danse";
    idle.instructions[2][0] = "Approchez\nEntrez dans la zone de danse";
    // idle.instructionVid[0][0][0] = ""; // intro
    // idle.instructionVid[0][0][1] = "video/text/COME_CLOSER_7_loop-photoJPEG.mov"; // loop
    // idle.instructionVid[0][0][2] = ""; // outro

    // idle.instructionVid[1][0][0] = ""; // intro
    // idle.instructionVid[1][0][1] = "video/text/COME_CLOSER_7-FR_loop-photoJPEG.mov"; // loop
    // idle.instructionVid[1][0][2] = ""; // outro
    idle.length[0] = -1;
    scenes[n] = idle;


    n++;

    sceneInfo stars;
    stars.name = "Stars";
    stars.no = n;
    stars.segments = 4;
    s = 0;
    stars.instructions[0][s] = " ";
    stars.instructions[1][s] = " ";
    stars.instructions[2][s] = " ";
    stars.analysis[s] = " ";
    stars.length[s] = 30;
    s = 1; 
    stars.instructions[0][s] = " ";
    stars.instructions[1][s] = " ";
    stars.instructions[2][s] = " ";
    stars.analysis[s] = " ";
    stars.length[s] = 30;
    s = 2;
    stars.instructions[0][s] = "Stand still";
    stars.instructions[1][s] = "Ne bougez plus";
    stars.instructions[2][s] = "Ne bougez plus";
    stars.analysis[s] = "* Velocity < FreezeMaxVel\n* frozenTimer > freezeMinTime\n-> 10 sec || all frozen";
    stars.length[s] = 5;
    s = 3;
    stars.instructions[0][s] = "Try new spots\nto light up more stars";
    stars.instructions[1][s] = "Posez-vous à de nouveaux endroits\npour allumer de nouvelles étoiles";
    stars.instructions[2][s] = "Posez-vous a de nouveaux endroits\npour allumer de nouvelles etoiles";
    stars.analysis[s] = "* Velocity < FreezeMaxVel\n* frozenTimer > freezeMinTime\n* frozenTimer < freezeMaxTime\n+ Star Animation at end of freezeMaxTime\n-> newStarMax stars || 40 sec";
    stars.length[s] = 40;   // FOR DEBUGGING
    scenes[n] = stars;

    n++;

    sceneInfo constellations;
    stars.name = "Attraction";
    stars.no = n;
    stars.segments = 2;
    stars.instructions[0][0] = "Walk with someone\nKeep the same distance\nbetween you\n(No hands!)";
    stars.instructions[1][0] = "Marchez avec quelqu'un\nGardez toujours le même écart\n(Sans les mains!)";
    stars.instructions[2][0] = "Marchez avec quelqu'un\nGardez toujours le meme ecart\n(Sans les mains!)";
    stars.analysis[0] = "* velocity history > movingThr\n* distance history < distStdDevThr\n-> 35 sec";
    stars.length[0] = 30;
    stars.instructions[0][1] = "Walk with someone\nMake eye contact\nKeep the distance";
    stars.instructions[1][1] = "Marchez avec quelqu'un\nRegardez-vous dans les yeux\nGardez toujours le même écart";
    stars.instructions[2][1] = "Marchez avec quelqu'un\nRegardez-vous dans les yeux\nGardez toujours le meme ecart";
    stars.analysis[1] = "* velocity history > movingThr\n* distance history < distStdDevThr\n-> 30 sec";
    stars.length[1] = 30;
    scenes[n] = stars;

    n++;

    sceneInfo revolution;
    revolution.name = "Revolutions";
    revolution.no = n;
    revolution.segments = 2;
    revolution.instructions[0][0] = "Take someone's hand\nSpin and lean out\nas far as possible";
    revolution.instructions[1][0] = "Prenez quelqu'un par les mains\nTournez ensemble en laissant tomber\nvos têtes vers l'arrière";
    revolution.instructions[2][0] = "Prenez quelqu'un par les mains\nTournez ensemble en laissant tomber\nvos tetes vers l'arriere";
    revolution.analysis[0] = "* activityCnt >= spinSuccess\n-> 20 sec";
    revolution.length[0] = 20;
    revolution.instructions[0][1] = "";
    // revolution.instructionImg[0][1] = "LET-GO-BLACK.jpg";
    revolution.instructionVid[0][1][0] = "video/text/LET_GO_8_intro-photoJPEG.mov"; // intro
    revolution.instructionVid[0][1][1] = "video/text/LET_GO_8_loop-photoJPEG.mov"; // loop
    revolution.instructionVid[0][1][2] = ""; // outro
    revolution.instructions[1][1] = "";
    revolution.instructions[2][1] = "";
    // revolution.instructionImg[1][1] = "LACHEZ-TOUT-BLACK.jpg";
    revolution.instructionVid[1][1][0] = "video/text/LET_GO_8-FR_intro-photoJPEG.mov"; // intro
    revolution.instructionVid[1][1][1] = "video/text/LET_GO_8-FR_loop-photoJPEG.mov"; // loop
    revolution.instructionVid[1][1][2] = ""; // outro
    revolution.analysis[1] = "* activityCnt < spinFailure\n-> 10 sec";
    revolution.length[1] = 10;
    scenes[n] = revolution;

    n++;

    sceneInfo sun;
    sun.name = "Sun";
    sun.no = n;
    sun.segments = 5;
    sun.instructions[0][0] = "Stand on one leg";
    sun.instructions[1][0] = "Tenez-vous sur une jambe";
    sun.instructions[2][0] = "Tenez-vous sur une jambe";
    sun.analysis[0] = "- \n-> 5 sec";
    sun.length[0] = 5;
    sun.instructions[0][1] = "Hop from one\nspot to the other";
    sun.instructions[1][1] = "Déplacez-vous en sautant\nd'un pied à l'autre";
    sun.instructions[2][1] = "Deplacez-vous en sautant\nd'un pied a l'autre";
    sun.analysis[1] = "* onLost event\n-> 20 sec";
    sun.length[1] = 20;
    // sun.instructions[0][2] = "Everyone in unison";
    // sun.instructions[1][2] = "Tout le monde à l'unisson";
    // sun.analysis[2] = "* onLost event\n-> 20 sec";
    // sun.length[2] = 20;
    sun.instructions[0][2] = "";
    // sun.instructionImg[0][2] = "FREEZE-BLACK.jpg";
    sun.instructionVid[0][2][0] = ""; // intro
    sun.instructionVid[0][2][1] = "video/text/FREEZE_4_loop-photoJPEG.mov"; // loop
    sun.instructionVid[0][2][2] = "video/text/FREEZE_4_static-photoJPEG.mov"; // outro
    sun.instructions[1][2] = "";
    sun.instructions[2][2] = "";
    // sun.instructionImg[1][2] = "STOP-BLACK.jpg";
    sun.instructionVid[1][2][0] = ""; // intro
    sun.instructionVid[1][2][1] = "video/text/FREEZE_4-FR_loop-photoJPEG.mov"; // loop
    sun.instructionVid[1][2][2] = "video/text/FREEZE_4-FR_static-photoJPEG.mov"; // outro
    sun.analysis[2] = "* hogAvVel < freezeAllMaxVel\n* segmentClock > 3\n-> 8 sec || all frozen";
    sun.length[2] = 8;
    sun.instructions[0][3] = "Run in every\ndirection at once.";
    sun.instructions[1][3] = "Courez partout en même temps";
    sun.instructions[2][3] = "Courez partout en meme temps";
    sun.analysis[3] = "* hogAvVel > runHogThr \n  || activityCnt > runActThr\n-> 30 sec";
    sun.length[3] = 10;
    sun.instructions[0][4] = "everyone";
    sun.instructions[1][4] = "tout le monde";
    sun.instructions[2][4] = "tout le monde";
    // sun.instructionImg[0][4] = "FREEZE-BLACK.jpg";
    sun.instructionVid[0][4][0] = ""; // intro
    sun.instructionVid[0][4][1] = "video/text/FREEZE_4_loop-photoJPEG.mov"; // loop
    sun.instructionVid[0][4][2] = "video/text/FREEZE_4_static-photoJPEG.mov"; // outro
    // sun.instructionImg[1][4] = "STOP-BLACK.jpg";
    sun.instructionVid[1][4][0] = ""; // intro
    sun.instructionVid[1][4][1] = "video/text/FREEZE_4-FR_loop-photoJPEG.mov"; // loop
    sun.instructionVid[1][4][2] = "video/text/FREEZE_4-FR_static-photoJPEG.mov"; // outro
    sun.analysis[4] = "* hogAvVel < freezeAllMaxVel\n* segmentClock > 3\n-> 8 sec || all frozen";
    sun.length[4] = 8;
    scenes[n] = sun;

    n++;

    s = 0;
    sceneInfo eclipse;
    eclipse.name = "Eclipse";
    eclipse.no = n;
    eclipse.segments = 8;
    s = 0;
    eclipse.instructions[0][s] = "Now line-up in\nfront of me";
    eclipse.instructions[1][s] = "Faites une file face à moi";
    eclipse.instructions[2][s] = "Faites une file face a moi";
    eclipse.analysis[s] = "- \n-> 15 sec";
    eclipse.length[s] = 15;
    s = 1;
    eclipse.instructions[0][s] = "Follow me";
    eclipse.instructions[1][s] = "Suivez moi";
    eclipse.instructions[2][s] = "Suivez moi";
    eclipse.analysis[s] = "\n-> 30sec || sinus 0";
    eclipse.length[s] = 30;
    s = 2;
    eclipse.instructions[0][s] = "Get back into the line";
    eclipse.instructions[1][s] = "Refaites une file";
    eclipse.instructions[2][s] = "Refaites une file";
    eclipse.analysis[s] = "* x == main.x\n-> 7 sec";
    eclipse.length[s] = 7;
    s = 3;
    eclipse.instructions[0][s] = "Step out of the line";
    eclipse.instructions[1][s] = "Sortez de la file";
    eclipse.instructions[2][s] = "Sortez de la file";
    eclipse.analysis[s] = "\n-> 7 sec ";
    eclipse.length[s] = 7;
    s = 4;
    eclipse.instructions[0][s] = "Step into the line";
    eclipse.instructions[1][s] = "Revenez dans la file";
    eclipse.instructions[2][s] = "Revenez dans la file";
    eclipse.analysis[s] = "* x == main.x\n-> 7 sec";
    eclipse.length[s] = 7;
    s = 5;
    eclipse.instructions[0][s] = "Step out of the line";
    eclipse.instructions[1][s] = "Sortez de la file";
    eclipse.instructions[2][s] = "Sortez de la file";
    eclipse.analysis[s] = "\n-> 7 sec";
    eclipse.length[s] = 7;
    s = 6;
    eclipse.instructions[0][s] = "Step into the line";
    eclipse.instructions[1][s] = "Revenez dans la file";
    eclipse.instructions[2][s] = "Revenez dans la file";
    eclipse.analysis[s] = "* x == main.x\n-> 7 sec ";
    eclipse.length[s] = 7;
    s = 7;
    eclipse.instructions[0][s] = "Disperse very slowly\ntowards the edges";
    eclipse.instructions[1][s] = "Dispersez-vous\ntrès lentement";
    eclipse.instructions[2][s] = "Dispersez-vous\ntres lentement";
    eclipse.analysis[s] = "* edge-proximity = opacity\n-> 15 sec";
    eclipse.length[s] = 15;
    scenes[n] = eclipse;

    n++;

    sceneInfo shooting;
    shooting.name = "Shooting Stars";
    shooting.no = n;
    shooting.segments = 5;
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
    // shooting.instructionImg[0][2] = "look-at-the-sky-BLACK.jpg";
    shooting.instructionVid[0][2][0] = "video/text/EXHALE_7_intro-photoJPEG.mov"; // intro
    shooting.instructionVid[0][2][1] = "video/text/EXHALE_7_loop-photoJPEG.mov"; // loop
    shooting.instructionVid[0][2][2] = "video/text/EXHALE_7_outro-photoJPEG.mov"; // outro
    // shooting.instructionVid[0][2][0] = sampleVideoFile; // intro
    // shooting.instructionVid[0][2][1] = sampleVideoFile; // loop
    // shooting.instructionVid[0][2][2] = sampleVideoFile; // outro
    shooting.instructions[1][2] = "";
    shooting.instructions[2][2] = "";
    // shooting.instructionImg[1][2] = "regardez-le-ciel-BLACK.jpg";
    shooting.instructionVid[1][2][0] = "video/text/EXHALE_7-FR_intro-photoJPEG.mov"; // intro
    shooting.instructionVid[1][2][1] = "video/text/EXHALE_7-FR_loop-photoJPEG.mov"; // loop
    shooting.instructionVid[1][2][2] = "video/text/EXHALE_7-FR_outro-photoJPEG.mov"; // outro
    // shooting.instructionVid[1][2][0] = sampleVideoFile; // intro
    // shooting.instructionVid[1][2][1] = sampleVideoFile; // loop
    // shooting.instructionVid[1][2][2] = sampleVideoFile; // outro
    shooting.analysis[2] = "- \n-> 10 sec";
    shooting.length[2] = 10;
    
    shooting.instructions[0][3] = "Stand up";
    shooting.instructions[1][3] = "Relevez-vous";
    shooting.instructions[2][3] = "Relevez-vous";
    shooting.analysis[3] = "- \n-> 10 sec";
    shooting.length[3] = 10;    
    s = 4;
    shooting.instructions[0][s] = " ";
    shooting.instructions[1][s] = " ";
    shooting.instructions[2][s] = " ";
    shooting.analysis[s] = " ";
    shooting.length[s] = 30;

    scenes[n] = shooting;

    ofLogNotice("START") << "\t" << ofGetFrameNum() << "\t" << "there are " << scenes.size() << " scenes";


    // initialize current scene/segment based on data saved in currentState.xml! << last scene/segment after termination
    scene = guiScene;
    segment = guiSegment;
    if (scene>=scenes.size()-1 && segment>=scenes[scene].segments-1) {
        ofLogNotice("START") << "\t" << ofGetFrameNum() << "\t" << "restarted at normal position: scene " << scene << " : " << segment;
        scene = 0;
        segment = 0;
        languageChange();
    } else {
        ofLogNotice("START") << "\t" << ofGetFrameNum() << "\t" << "untypical termination, restart at: scene " << scene << " : " << segment;
    }


    sceneChange = true;
    nextSegment();
    (*bgMedia[bgMediaId]).id = bgMediaId;
    (*bgMedia[bgMediaId]).reset(true);

}

//--------------------------------------------------------------
void planeApp::update(){

    // ofLogNotice("interaction") << "\t" << ofGetFrameNum() << "\t" << "start update()";

    // ofGetFrameRate freaks out if at start of application? 
    if (processing) {

        // check for incoming OSC messages
        oscMsgReceived = false;    
        this->receiveOsc();     // sets oscMsgReceived to true, if blobserver/endFrame 
        oscLastMsgTimer = ofGetElapsedTimef() - oscLastMsg;
        if (!oscMsgReceived && oscActive && oscLastMsgTimer > oscLastMsgTimerMax) {
            oscActive = false;
        }

        if (oscMsgReceived) {

        	// BLOB CLEANUP
            std::map<int,Blob>::iterator it = blobs.begin();
            while (it != blobs.end()) {
                Blob* b = &it->second;

                int minLost = minLostTime;
                if (scene==STARS || scene==ATTRACTION) minLost = minLostSpin;
                else if (scene==SUN) minLost = minLostHop;
                else if (scene==SHOOTING) minLost = minLostShoot;

                // position averaging, perspective transform, and position history
                b->processRawPosition(siteW, siteH, stageRadius, y_mean);
                // check lifetime and if lost, trigger onLost event
            	b->update(minLost);

                // delete blob if it hasn't been updated and its lifetime has passed
            	if( !b->isAlive() ) {
                    ofLogNotice("BLOB") << "\t\t" << ofGetFrameNum() << "\t" << "dead\t" << b->id;
                    ofNotifyEvent(b->prepareToDie,b->id,b);
            		blobs.erase(it++);
            	} else {
            		++it;
            	}
            }
            // ofLogNotice("BLOB") << "\t\t" << ofGetFrameNum() << "\t" << "cleand up blobs >> " << blobs.size();

            // ANALYSIS
            // store all the blob position in a data structure that can be passed back into all blobs
            std::map<int, tinyNeighbor> blobPositions;
            for(std::map<int, Blob>::iterator it = blobs.begin(); it != blobs.end(); ++it){
                Blob* b = &it->second;
                blobPositions[b->id].set(b->position, b->movingMean, b->onStage);
            }
            // analysis based on velocity (frozen, freeze events, mean velocity, ..)
            // TODO WHY IS THIS HERE, AND NOT IN THE LOOP ABOVE??
            for(std::map<int, Blob>::iterator it = blobs.begin(); it != blobs.end(); ++it){
                Blob* b = &it->second;
                b->analyze(freezeMaxVel, freezeMinTime, freezeMaxTime, movingThr);
            }
            // analyse blob distance to its neighbors (steady, break steady events)
            if (scene==ATTRACTION) {
                for(std::map<int, Blob>::iterator it = blobs.begin(); it != blobs.end(); ++it){
                    Blob* b = &it->second;
                    b->analyzeNeighbors(blobPositions, distStdDevThr);
                }
            }
            // ofLogNotice("BLOB") << "\t\t" << ofGetFrameNum() << "\t" << "analyzed blobs >> " << blobs.size();

            // determine how many blobs are on stage, and their average velocity
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

            // now all blobs with attached video, shall update the video location
            // ofLogNotice("TRANSITION") << "\t" << ofGetFrameNum() << "\t" << "update video";
            it = blobs.begin();
            while (it != blobs.end()) {
                Blob* b = &it->second;
                b->updateVideo();
                it++;
            }
            // ofLogNotice("BLOB") << "\t\t" << ofGetFrameNum() << "\t" << "update blobs video >> " << blobs.size();
        }

        // SCHEDULING
        masterClock = ofGetUnixTime() - globalStart;
        segmentClock = ofGetUnixTime() - segmentStart;
        lastActivity = ofGetUnixTime() - lastActivityClock;
        // if (flash) {
        //     flashCnt++;
        //     if (flashCnt > flashMax) {
        //         flashCnt = 0;
        //         flash = false;
        //         beginSegment();
        //     }
        // }
        // TIME TRIGGERS
        if (autoplay && scenes[scene].length[segment] > 0 && segmentClock >= scenes[scene].length[segment] && !transition) {
            endSegment();
        }
        // FAIL SAVE TIME TRIGGER
        if (autoplay && scenes[scene].length[segment] > 0 && segmentClock >= scenes[scene].length[segment]+10) {
            ofLogError("TRANSITION") << "\t" << ofGetFrameNum() << "\t" << "FORCE transition change, because segmentClock>+15";
            endSegment();        // FOR DEBUGGING
        }
        if (autoplay && scenes[scene].length[segment] > 0 && segmentClock >= scenes[scene].length[segment]+20) {
            ofLogError("TRANSITION") << "\t" << ofGetFrameNum() << "\t" << "FORCE transition change, because segmentClock>+20";
            moveOn = true;       // FOR DEBUGGING
        }
        // triggered by the end of fading out the bg
        if (moveOn) {
            nextSegment();
            moveOn = false;
        }

        // TRANSITIONING BETWEEN SEGMENT RELATED ACTION
        if (scene==IDLE) {
            // fade out idle-mode video, connect fade-End to transition to next Segment
            if(autoplay && success && !transition) {
                ofLogNotice("interaction") << "\t" << ofGetFrameNum() << "\t" << "scene " << scene << " success";
                endSegment();
            }
        } else if (scene==STARS) {
            if (autoplay && !transition && segmentClock > minSegmentLength) {
                if (segment==SEG_STARS && hogAvVel < freezeAllMaxVel) {
                    ofLogNotice("interaction") << "\t" << ofGetFrameNum() << "\t" << "scene " << scene << " segment 0 success: all frozen";
                    endSegment();
                } else if (segment==SEG_CONSTELLATIONS && successCnt > newStarMax) {
                    ofLogNotice("interaction") << "\t" << ofGetFrameNum() << "\t" << "scene " << scene << " segment 1 success: maxStarCnt";
                    endSegment();
                }
            }
        } else if (scene==REVOLUTIONS && segment==1) {
            if (autoplay && success && !transition && segmentClock > minSegmentLength) {
                ofLogNotice("interaction") << "\t" << ofGetFrameNum() << "\t" << "scene " << scene << " segment 1 success";
                endSegment();
            }
        } else if (scene==SUN) {
            if (segment==2 || segment==4 ) {
                // FREEZE!
                if (hogAvVel < freezeAllMaxVel && segmentClock > 3) {
                    success = true;
                } else {
                    // once freeze-success, no way back to false,
                    // so that this doesn't change, while the system is busy fading out the instructions
                    if (segmentClock < minSegmentLength) success = false;
                }
                if(autoplay && success && !transition && segmentClock > minSegmentLength) {
                    ofLogNotice("interaction") << "\t" << ofGetFrameNum() << "\t" << "FREEZE success";
                    endSegment();
                }
            }
        } else if (scene==ECLIPSE) {
            if (autoplay && success && !transition && segmentClock > minSegmentLength ) {
                if (segment==3 || segment==5) {
                    // STEP OUT OF THE LINE
                    // if (blobsOnStage > 0 && !success) endSegment();
                } else if (segment==2 || segment==4 || segment==6) {
                    // STEP INTO THE LINE
                    // if (success) endSegment();
                }
            }
        }



        float updateRate  = 30.0/ofGetFrameRate();  
        // float updateRate  = 60.0/ofGetFrameRate();  
        // ofLogNotice("interaction") << "\t" << ofGetFrameNum() << "\t" << "updateRate " << updateRate;
        if (updateRate>0.5 && updateRate<6) {

            // animation relatedspinJT
            if (scene==REVOLUTIONS) {

                int spinJT = int ( float(spinJudgeTime) / updateRate);   // half framerate --> 2.0
                // 30.0/ofGetFrameRate();

                if (!transition && ofGetFrameNum()%spinJT==0) {
                    ofLogNotice("interaction") << "\t" << ofGetFrameNum() << "\t" << "\tspinJudgeTime: " << spinJT << " (" << spinJudgeTime << ")";
                    if (segment==0) {
                        int oldCnt = planetCnt;
                        // set planetCnt based on activityCnt
                        if (activityCnt >= spinSuccess && fgMedia.size()<5) planetCnt++;
                        else if (activityCnt < spinFailure) planetCnt--;

                        if (activityCnt==0) planetCnt=0;
                        if (planetCnt>((blobsOnStage+1)/2.0)) planetCnt = ((blobsOnStage+1)/2);
                        if (planetCnt>5) planetCnt = 5;   // there might still be squiggels, that take planet space!
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
                                planetCnt = oldCnt + 1;
                                ofLogNotice("interaction") << "\t" << ofGetFrameNum() << "\t" << "\tadd a planet, planetCnt " << planetCnt;
                            } else {
                                // TAKE AWAY PLANETS
                                ofLogNotice("interaction") << "\t" << ofGetFrameNum() << "\t" << "\ttake away planet, planetCnt " << planetCnt;
                                if (fgMedia.size()>0) {
                                    for (unsigned int i=fgMedia.size()-1; i>=0; i--) {
                                        if (i>=0 && !(*fgMedia[i]).selfdestroy) {
                                            ofLogNotice("interaction") << "\t" << ofGetFrameNum() << "\t" << "\ttake away planet " << i;
                                            (*fgMedia[i]).loadMovie("video/3_revolution/REV_OUT-photoJPEG.mov");
                                            (*fgMedia[i]).reset(true);
                                            (*fgMedia[i]).autoDestroy(true);
                                            // if (i<=planetCnt) break;
                                            break;  // only take away one planet at a time = safer
                                        }
                                    }
                                }
                            }

                            positionRevolutions();
                        }

                    } else if (segment==1) {
                        int oldCnt = planetCnt;
                        if (activityCnt < spinFailure) planetCnt--;
                        if (planetCnt<=-1) {
                            success = true;
                        } else if (planetCnt<oldCnt) {
                            ofLogNotice("interaction") << "\t" << ofGetFrameNum() << "\t" << "\ttake away planet, planetCnt " << planetCnt;
                            if (fgMedia.size()>0) {
                                for (unsigned int i=fgMedia.size()-1; i>=0; i--) {
                                    if (i>=0 && !(*fgMedia[i]).selfdestroy) {
                                        ofLogNotice("interaction") << "\t" << ofGetFrameNum() << "\t" << "\ttake away planet " << i;
                                        (*fgMedia[i]).loadMovie("video/3_revolution/REV_OUT-photoJPEG.mov");
                                        (*fgMedia[i]).reset(true);
                                        (*fgMedia[i]).autoDestroy(true);
                                        // if (i>=planetCnt) break;
                                        break;  // only take away one planet at a time = safer
                                    }
                                }
                            }
                            // if (planetCnt<=0) success = true;
                        }
                    }
                    ofLogNotice("interaction") << "\t" << ofGetFrameNum() << "\t" << "\tactivityCnt\t" << activityCnt << "\tplanetCnt\t" << planetCnt;
                    activityCnt = 0;
                }

            } else if (scene==SUN && segment==3) {
                int runJT = int ( float(runJudgeTime) / updateRate);
                if (!transition && ofGetFrameNum()%runJT==0) {
                    // if high activity, add blue surface to sun
                    // DEBUGGING KILL
                    // if (hogAvVel > runHogThr || activityCnt > runActThr) {
                    //     // activity!
                    //     // int rndBlueSun = ofRandom(sun_surface_blue.size());
                    //     int rndBlueSun = ofRandom(3) + 1;
                    //     // fgMedia.push_back(sun_surface_blue[rndBlueSun]);   // photoJPEG   SUN_run_surface-1-blue-qtPNG.mov  SUN_run_surface-1-blue-photoJPEG.mov
                    //     // fgMedia.push_back(ofPtr<mediaElement>( new videoElement("video/4_sun/SUN_run_surface-"+ofToString(rndBlueSun)+"-blue-qtPNG.mov",false)));
                    //     fgMedia.push_back(ofPtr<mediaElement>( new videoElement("video/4_sun/SUN_run_surface-4-blue-qtPNG.mov",false)));
                    //     (*fgMedia[fgMedia.size()-1]).setDisplay(projectionW/2,projectionH/2, true);
                    //     (*fgMedia[fgMedia.size()-1]).reset();
                    //     (*fgMedia[fgMedia.size()-1]).autoDestroy(true);
                    //     (*fgMedia[fgMedia.size()-1]).movieEndTrigger=true;
                    //     ofAddListener( (*fgMedia[fgMedia.size()-1]).fadeOutEnd, this, &planeApp::unHideSun );
                    //     ofLogNotice("interaction") << "\t" << ofGetFrameNum() << "\t" << "\trunJudge:  activity!  hog: "<< hogAvVel << "\tact: " << activityCnt;
                    //     fgMedia[0]->hide = true;
                    // } else {
                    //     ofLogNotice("interaction") << "\t" << ofGetFrameNum() << "\t" << "\trunJudge:  not enough activity";
                    // }

                    activityCnt = 0;
                }
            } else if (scene==ECLIPSE && segment==SEG_FOLLOWME) {
                // FOLLOW ME
                followMe += followMeSpeed * updateRate;
                if (followMe >= 2*3.14) {
                    (*fgMedia[0]).position.x = projectionW/2 + moonPosX;
                    // planet back at center position, move on to next scene
                    if (followMe > 2*3.6 && autoplay && !transition) {
                        ofLogNotice("interaction") << "\t" << ofGetFrameNum() << "\t" << "end because folloMe " << followMe;
                        endSegment();
                    }
                } else if (followMe > 0) {
                    (*fgMedia[0]).position.x = projectionW/2 + moonPosX + sin(followMe) * followMeRadius;
                    (*fgMedia[1]).position.x = (*fgMedia[0]).position.x;
                    // position GLOW video, if exists
                    // if (fgMedia.size() > 0) {
                    //     for (unsigned int i=1; i<fgMedia.size(); i++) {
                    //         if ((*fgMedia[i]).file == "video/5_eclipse/LIGHT-photoJPEG.mov") {
                    //             (*fgMedia[i]).position.x = (*fgMedia[0]).position.x;
                    //         }
                    //     }
                    // }
                }
            }

            if ( scene==ECLIPSE && segment<scenes[scene].segments-1 && !transition && segmentClock > minSegmentLength && blobsOnStage>0) {

                // check if all are aligned
                bool allAligned = allBlobsAlignedWith((*fgMedia[0]).position);

                if (allAligned) {
                    // (*fgMedia[1]).hide = false;
                    if (!success) {
                    //     // aligned
                        success = true;
                        // (*fgMedia[1]).fadeIn();
                        (*fgMedia[1]).fadeOut(-0.1, (*fgMedia[1]).opacity, false);
                    //     string alignmentGlow = "video/5_eclipse/LIGHT-photoJPEG.mov";
                    //     fgMedia.push_back(ofPtr<mediaElement>( new videoElement(alignmentGlow, true) ));
                    //     (*fgMedia[fgMedia.size()-1]).setDisplay((*fgMedia[0]).position.x, (*fgMedia[0]).position.y, true);
                    //     (*fgMedia[fgMedia.size()-1]).reset(true);
                    //     (*fgMedia[fgMedia.size()-1]).autoDestroy(true);
                    }
                } else {
                    // (*fgMedia[1]).hide = true;
                    if (success) {
                        success = false;
                        // (*fgMedia[1]).fadeOut();
                        (*fgMedia[1]).fadeOut(0.1, (*fgMedia[1]).opacity, false);
                    }
                }
            }



            // VIDEO
            nebula->update(updateRate);
            (*bgMedia[bgMediaId]).update(updateRate);
            instructionVid->update(updateRate);
            instructionTxt.update(updateRate);

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
                (**iter).update(updateRate);
                if ((**iter).dead) {
                    ofLogNotice("videoElement") << ofGetFrameNum() << "\t" << "delete video " << (**iter).file;
                    iter = fgMedia.erase(iter++);
                } else {
                    ++iter;
                }
            }
            // ofLogNotice("videoElement") << ofGetFrameNum() << "\t" << "cleand up videos";
        } else ofLogNotice("interaction") << "\t" << ofGetFrameNum() << "\t" << "updateRate crazy: " << updateRate;

    }
    // ofLogNotice("interaction") << "\t" << ofGetFrameNum() << "\t" << "finish update()";

}

/* define the current bg videofile, depending on scene and segment */
void planeApp::bgMediaSwap(int & trans) {

    // (*bgMedia[bgMediaId]).reset(true);

    if (scene==IDLE && ((*bgMedia[bgMediaId]).id==-1 || (*bgMedia[bgMediaId]).id==5)) {
        ofLogNotice("TRANSITION") << "\t" << ofGetFrameNum() << "\t" << "bgMediaSwap\t\t\tidle " << trans;
        bgMediaId = ofRandom(4);
        (*bgMedia[bgMediaId]).id = bgMediaId;
        (*bgMedia[bgMediaId]).reset(true);   // play right away
        (*bgMedia[bgMediaId]).movieEndTrigger=false;
        (*bgMedia[bgMediaId]).outroTransformation = &mediaElement::finishMovie;
        ofAddListener( (*bgMedia[bgMediaId]).fadeOutEnd, this, &planeApp::bgMediaSwap );
    } else if (scene==IDLE && (*bgMedia[bgMediaId]).id<4) {
        ofLogNotice("TRANSITION") << "\t" << ofGetFrameNum() << "\t" << "bgMediaSwap\t\t\tstarry intro " << trans;
        bgMediaId = 4;
        (*bgMedia[bgMediaId]).id = bgMediaId;
        (*bgMedia[bgMediaId]).reset(true);
        (*bgMedia[bgMediaId]).finishMovie(1.0);
        // (*bgMedia[bgMediaId]).movieEndTrigger=true;
        ofAddListener( (*bgMedia[bgMediaId]).fadeOutEnd, this, &planeApp::bgMediaSwap );
    } else if (scene==REVOLUTIONS && (*bgMedia[bgMediaId]).id==5) {
        ofLogNotice("TRANSITION") << "\t" << ofGetFrameNum() << "\t" << "bgMediaSwap\t\t\tsun intro " << trans;
        bgMediaId = 6;
        (*bgMedia[bgMediaId]).id = bgMediaId;
        (*bgMedia[bgMediaId]).reset(true);
        (*bgMedia[bgMediaId]).finishMovie(1.0);
        // (*bgMedia[bgMediaId]).movieEndTrigger=true;
        ofAddListener( (*bgMedia[bgMediaId]).fadeOutEnd, this, &planeApp::bgMediaSwap );
    } else {
        ofLogNotice("TRANSITION") << "\t" << ofGetFrameNum() << "\t" << "bgMediaSwap\t\t\tstarry bg " << trans;
        bgMediaId = 5;
        (*bgMedia[bgMediaId]).id = bgMediaId;
        (*bgMedia[bgMediaId]).reset(true);
        (*bgMedia[bgMediaId]).movieEndTrigger=false;
        ofRemoveListener( (*bgMedia[bgMediaId]).fadeOutEnd, this, &planeApp::bgMediaSwap );
        if (trans!=-2) nextSegment();
    }

}

void planeApp::allFaded(int & trans) {
    ofLogNotice("TRANSITION") << "\t" << ofGetFrameNum() << "\t" << "allFaded, moveOn = true";
    // set moveOn to true, instead of calling nextSegment()
    // to avoid conflicting threading
    moveOn = true;
}

void planeApp::unHideSun(int & trans) {

    if (scene==SUN && segment==3) {    // run everywhere
        ofLogNotice("mediaElement") << "\t" << ofGetFrameNum() << "\t" << "unHideSun";
        if (fgMedia.size()<3) {
            // make sure to uncover yellow sun only, if there was only 1 blue-surface
            fgMedia[0]->hide = false;
        }
    }
}


void planeApp::placeInstruction(int & trans) {
    if (scene==REVOLUTIONS && segment==1) {    // let go
        ofLogNotice("mediaElement") << "\t" << ofGetFrameNum() << "\t" << "placeInstruction";
        // place instruction vid based on planetCnt positioning
        float addY = 0;
        switch (planetCnt) {
            case 3: addY = -100;    // 3 > projectionH/2, +- 550
                    break;
            case 2: addY = -350;    // 2 > projectionH/2 +- 350
                    break;
            case 1: addY = 0;       // 1 > projectionH/2
        }
        addY = -100;
        instructionVid->setDisplay(projectionW/2,projectionH/2 + addY, true);
    }
}

// default:  attach event trigger 
// trans -1 : don't attach event trigger
void planeApp::fgMediaFadedOut(int & trans) {
    ofLogNotice("TRANSITION") << "\t" << ofGetFrameNum() << "\t" << "fgMediaFadedOut  " << trans << "  fgmedia size = " << fgMedia.size();


    if (fgMedia.size() > 0) {

        // FADE OUT ALL MEDIAelements AT ONCE
        bool attachedTrigger = false;
        for (vector<ofPtr<mediaElement> >::iterator it = fgMedia.begin(); it != fgMedia.end(); ++it) {
            // they could already be dead, or fading, or hidden (eclipse glow)
            if (!(**it).dead && !(**it).fading && !(**it).hide) {
                // outro = usually fadeOut, fade ends with triggering fadeOutEnd event
                ((((**it)).*((**it)).outroTransformation))();
                // attache trigger to first non-dead element
                if (trans>0 && !attachedTrigger) {
                    ofLogNotice("TRANSITION") << "\t" << ofGetFrameNum() << "\t" << "added allFaded() listener to " << (**it).file;
                    (**it).movieEndTrigger = true;
                    ofAddListener( (**it).fadeOutEnd, this, &planeApp::allFaded );
                    attachedTrigger = true;
                }
            } else {
                ofLogNotice("TRANSITION") << "\t" << ofGetFrameNum() << "\t" << (**it).file << " dead: " << (**it).dead << "  fading: " << (**it).fading;
            }
        }

        if (trans>0 && !attachedTrigger) {
            // assumes all mediaElements are as good as dead, therefore simply 
            ofLogNotice("TRANSITION") << "\t" << ofGetFrameNum() << "\t" << "all dead or fading already, let's moveOn";
            moveOn = true;
        }

    } else {
        if (scene==IDLE && (*bgMedia[bgMediaId]).mediaLoaded) {
            // make sure bgMedia is loaded, so the transition also works without the videofile
            ((*bgMedia[bgMediaId]).*((*bgMedia[bgMediaId]).outroTransformation))();
        } else {
            if (trans>=0) moveOn = true;
        }
    }
}


void planeApp::blobOnLost(int & blobID) {
    ofLogNotice("BLOB") << "\t\t" << ofGetFrameNum() << "\t" << blobID << " just got lost  (occl: " << blobs[blobID].occluded << ")";
    if (!transition && blobs[blobID].onStage) {
        if (scene==REVOLUTIONS) {
            activityCnt++;
        } else if (scene==SUN && !blobs[blobID].occluded) {
            // SUN explosions
            activityCnt++;
            if (segment==1) {
                int randomExpl = ofRandom(7) + 1;
                // SUN_explosion-02_fullscale-blue-posterized-centered-qtPNG.mov
                string videoEnd = "_fullscale-blue-posterized-centered-qtPNG.mov";
                string newVideoName = "video/4_sun/SUN_explosion-" + ofToString(randomExpl,2,'0') + videoEnd;
                fgMedia.push_back(ofPtr<mediaElement>( new videoElement(newVideoName,false)));
                (*fgMedia[fgMedia.size()-1]).setDisplay(blobMapToScreen(blobs[blobID].position), true);
                (*fgMedia[fgMedia.size()-1]).reset();
                (*fgMedia[fgMedia.size()-1]).autoDestroy(true);
                // if (segment==2) (*fgMedia[0]).bounce(); // sun video = [0]
            }
        } else if (scene==SHOOTING && !blobs[blobID].occluded) {
            // SHOOTING STARS
            if (segment==0) {
                if (++shootingPointer>=shooting_stars.size()) shootingPointer=0;
                fgMedia.push_back(shooting_stars[shootingPointer]);
                (*fgMedia[fgMedia.size()-1]).setDisplay(ofRandom(projectionW-100), ofRandom(projectionH-100), true);
                (*fgMedia[fgMedia.size()-1]).reset();
                (*fgMedia[fgMedia.size()-1]).moveAcross( blobs[blobID].velocity.x, blobs[blobID].velocity.y, projectionW, projectionH, false);
                (*fgMedia[fgMedia.size()-1]).autoDestroy(true);
                (*fgMedia[fgMedia.size()-1]).movie->setSpeed(0.5);
            } else if (segment==1) {
                float randdeg = ofRandom(-5.f, 5.f);
                for (int i=0; i<10; i++) {
                    if (++shootingPointer>=shooting_stars.size()) shootingPointer=0;
                    fgMedia.push_back(shooting_stars[shootingPointer]);
                    (*fgMedia[fgMedia.size()-1]).setDisplay(ofRandom(projectionW-100), ofRandom(projectionH-100), true);
                    (*fgMedia[fgMedia.size()-1]).reset();
                    (*fgMedia[fgMedia.size()-1]).moveAcross( randdeg, 45.f, projectionW, false);
                    (*fgMedia[fgMedia.size()-1]).autoDestroy(true);
                    (*fgMedia[fgMedia.size()-1]).movie->setSpeed(0.5);
                }
            }
        }
    }
}

void planeApp::blobSteady(Pair & pair) {
    // add particle trail video between stars
    if (!transition && scene==ATTRACTION && numBridges<10) {
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
                    ofLogNotice("BLOB") << "\t" << ofGetFrameNum() << "\t\t\t" << "make blob " << pair.blob1 << " sparklier";
                    (*vid1).loadMovie(videoFile);
                    (*vid1).reset();
                }
            }
            if (vid2 != NULL) {
                string videoFile = "video/2_stars/STAR_"+ofToString((*vid2).id)+"-glow-photoJPEG.mov";
                if ((*vid2).file != videoFile) {
                    ofLogNotice("BLOB") << "\t" << ofGetFrameNum() << "\t\t\t" << "make blob " << pair.blob2 << " sparklier";
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
                numBridges++;
                ofLogNotice("BLOB") << "\t" << ofGetFrameNum()  << "\t\t\t" << "add video bridge";
                float distance = ofDist(b1->position.x, b1->position.y, b2->position.x, b2->position.y);
                int distId = 1;
                if (distance < 150 ) distId = 2;    // TODO fix LINK_03 vid
                else if (distance < 300) distId = 2;
                string newVideoName = "video/2_stars/LINK_0" + ofToString(distId) + "-loop-photoJPEG.mov";
                fgMedia.push_back(ofPtr<mediaElement>( new videoElement(newVideoName)));
                (*fgMedia[fgMedia.size()-1]).reset();
                (*fgMedia[fgMedia.size()-1]).id = distId;
                (*fgMedia[fgMedia.size()-1]).fadeIn(0.2);
                (*fgMedia[fgMedia.size()-1]).bridge(b1->id, b2->id);
                ofNotifyEvent( blobs[pair.blob1].updatePosition, pair.blob1, &blobs[pair.blob1] );
            } else {
                ofLogNotice("BLOB") << "\t" << ofGetFrameNum()  << "\t\t\t" << "bridge already exists";
            }
        }
    }
}

void planeApp::blobSteadyReward(Pair & pair) {
    ofLogNotice("BLOB") << "\t\t" << ofGetFrameNum() << "\t" << "blobSteadyReward() \t" << pair.blob1 << " + " << pair.blob2;
    if (!transition && scene==ATTRACTION) {
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
    if (!transition && scene==ATTRACTION) {
        ofLogNotice("BLOB") << "\t\t" << ofGetFrameNum() << "\t" << "blobBreakSteady() \t" << pair.blob1 << " + " << pair.blob2;

        std::map<int,Blob>::iterator iter1 = blobs.find(pair.blob1);
        std::map<int,Blob>::iterator iter2 = blobs.find(pair.blob2);

        if (iter1!=blobs.end()) {
            ofPtr<mediaElement> vid1 = blobs[pair.blob1].mediaLink;

            // replace video with normal star
            if (vid1 != NULL && !blobs[pair.blob1].steadyRewarded) {
                string videoFile = "video/2_stars/STAR_" + ofToString((*vid1).id)+"-loop-photoJPEG.mov";
                if ((*vid1).file != videoFile) {
                    (*vid1).loadMovie("video/2_stars/STAR_" + ofToString((*vid1).id)+"-loop-photoJPEG.mov");
                    (*vid1).reset();
                }
            }
        }

        if (iter2!=blobs.end()) {
            ofPtr<mediaElement> vid2 = blobs[pair.blob2].mediaLink;

            // replace video with normal star
            if (vid2 != NULL && !blobs[pair.blob2].steadyRewarded) {
                string videoFile = "video/2_stars/STAR_" + ofToString((*vid2).id)+"-loop-photoJPEG.mov";
                if ((*vid2).file != videoFile) {
                    (*vid2).loadMovie("video/2_stars/STAR_" + ofToString((*vid2).id)+"-loop-photoJPEG.mov");
                    (*vid2).reset();
                }
            }
        }


        bridgeUnlink(pair);

    }
}




void planeApp::blobOnFreeze(int & blobID) {
    if (!transition) {
        ofPoint p = blobMapToScreen(blobs[blobID].position);
        ofLogNotice("BLOB") << "\t\t" << ofGetFrameNum() << "\t" << "blobOnFreeze()\t" << blobID << " at " << int(p.x) << "|" << int(p.y);
        if (scene==STARS && segment>=SEG_STARS) {
            if (blobs[blobID].onStage) {
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
                if (segment==SEG_CONSTELLATIONS) successCnt++;
            }
        }
    }
}



void planeApp::blobUnFreeze(int & blobID) {
    if (!transition) {
        if (scene==STARS && segment>=SEG_STARS) {
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
        } else if (scene==SUN) {
            // if ((segment==3 || segment==5)) {
            //     success = false;
            // }
        }
    }
}

void planeApp::blobOverFreeze(int & blobID) {
    if (!transition) {
        if (scene==STARS) {
            if (segment==SEG_CONSTELLATIONS && blobs[blobID].onStage && blobs[blobID].mediaLink != NULL) {

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
                // "CONSTELLATION_11-photoJPEG.mov",    // bold ok
                "CONSTELLATION_13-photoJPEG.mov",    // bold ok
                "CONSTELLATION_14-photoJPEG.mov",    // bold ok
                // "CONSTELLATION_15-photoJPEG.mov",    // bold ok
                "CONSTELLATION_16-photoJPEG.mov",    // bold ok
                // "CONSTELLATION_BOAT-photoJPEG.mov",
                "CONSTELLATION_CAR-photoJPEG.mov",
                "CONSTELLATION_CASSEROLLE-photoJPEG.mov",
                // "CONSTELLATION_CUBE-photoJPEG.mov",
                // "CONSTELLATION_FISH-photoJPEG.mov",
                "CONSTELLATION_HAND-photoJPEG.mov",
                // "CONSTELLATION_HEART2-photoJPEG.mov",
                // "CONSTELLATION_HOUSE-photoJPEG.mov",
                // "CONSTELLATION_MERMAID-photoJPEG.mov",
                "CONSTELLATION_STICKBOY-photoJPEG.mov"
                };

                // string constellations[] = { "CONSTELLATION_9-photoJPEG.mov" };

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
                    // {448,244},    // 11x:448, y:244
                    {88,40},    // 13 x:88, y:40
                    {136,60},    // 14 x:136, y:60
                    // {96,148},    // 15 x:96, y:148
                    {38,106},    // 16 x:38, y:106
                    // {382,46},
                    {174,44},
                    {44,86},
                    // {36,74},
                    // {264,188},
                    {65,450},
                    // {60,50},
                    // {220,332},
                    // {176,58},
                    {120,428}
                };

                // int constOrigin[][2] = { {54,228} };

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
    if (scene==IDLE && !transition) {
        // COME CLOSER, recognition of people == time to move on
        if (!success) success = true;

    } else if (scene==ATTRACTION && !transition) {
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
        // (*fgMedia[fgMedia.size()-1]).outroTransformation = &mediaElement::scaleAway;
    } else if (scene==ECLIPSE && !transition) {
        // PLANETS, not allowed to enter in 'disperse' segment   //  && segment<scenes[scene].segments-1
        ofLogNotice("BLOB") << "\t\t" << ofGetFrameNum() << "\t" << "blobEnterStage()\t\t" << blobID << " (planet)";
        // int planedId[] = { 6, 9, 13, 15, 18, 19,20, 22, 23 };
        // int planets = sizeof(planedId) / sizeof(planedId[0]);
        pickPlanet++;
        // if (pickPlanet >= planets) pickPlanet = 0;
        if (pickPlanet > 29) pickPlanet = 0;
        // fgMedia.push_back(ofPtr<mediaElement>( new videoElement("video/5_eclipse/P_" + ofToString(planedId[pickPlanet])+"-qtPNG.mov", true)));
        fgMedia.push_back(ofPtr<mediaElement>( new imageElement("video/5_eclipse/ECLIPSE_planet_" + ofToString(pickPlanet+1)+".png",1)));
        fgMedia[fgMedia.size()-1]->blend = false;
        // fgMedia.push_back(planet_animated[pickPlanet]);
        blobs[blobID].mediaLink = fgMedia[fgMedia.size()-1];
        blobs[blobID].videoTrace = true;
        (*fgMedia[fgMedia.size()-1]).setDisplay(blobMapToScreen(blobs[blobID].position), true);
        (*fgMedia[fgMedia.size()-1]).reset();
        // fgMedia[fgMedia.size()-1]->moveElement = true;

        if (segment==0 && segmentClock < alignmentTransition) {   // 
            ofLogNotice("BLOB") << "\t\t" << ofGetFrameNum() << "\t" << "moveinfromside";
            (*fgMedia[fgMedia.size()-1]).moveInFromSide(projectionW/2,projectionH/2);
        } else {
            // (*fgMedia[fgMedia.size()-1]).fadeIn(0.1);
        }
    }
}

void planeApp::blobLeaveStage(int & blobID) {
    if (!transition && scene==ATTRACTION) {
        // STAR BRIDGES
        ofLogNotice("BLOB") << "\t\t" << ofGetFrameNum() << "\t" << "blobLeaveStage()\t\t" << blobID;
        this->blobUnlink(blobID);
    } else if (scene==ECLIPSE) {
        // PLANETS
        ofLogNotice("BLOB") << "\t\t" << ofGetFrameNum() << "\t" << "blobLeaveStage()\t\t" << blobID;
        this->blobUnlink(blobID);
    }
}

void planeApp::blobOnCreate(int & blobID) {
    ofLogNotice("BLOB") << "\t\t" << ofGetFrameNum() << "\t" << "blobOnCreate() \t" << blobID;

    if (!transition) {
        // first clean up, all related blob settings
        this->blobUnlink(blobID);
        blobs[blobID].videoTrace = false;
        blobs[blobID].mediaLink = ofPtr<mediaElement>();  // TODO how to release ofPtr ?

        blobs[blobID].onStage = false;
        // if (blobs[blobID].onStage) {         // too early here, for new blobs at least
        //     this->blobEnterStage(blobID);    // this will happen inside processRawPosition()
        // }
    } else {
        ofLogNotice("BLOB") << "\t" << ofGetFrameNum() << "\t\t\t" << "-";
    }
}

//
void planeApp::positionRevolutions() {

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
    if (blobs[blobID].onStage) {
        ofPtr<mediaElement> vid = blobs[blobID].mediaLink;
        // update position
        ofPoint p;
        if (vid != NULL) {
            // ofLogNotice("BLOB") << "\t\t" << ofGetFrameNum() << "\t" << "videoFollowBlob\t" << blobID;

            if (scene==ECLIPSE && segment>13 && segment<16) {
                // PLANETS aligned on VERTICAL, move up down
                p.y = blobMapToScreen(blobs[blobID].position).x * (16.0/9.0);
                p.x = offsetX + projectionW/2.0;
            } else if (scene==ECLIPSE && segment<scenes[scene].segments-1) {
                // PLANETS aligned on HORIZONTAL
                p.y = projectionH/2;
                p.x = blobMapToScreen(blobs[blobID].position).x * 0.8 + projectionW*0.1;
            } else if (!transition) {
                p = blobMapToScreen(blobs[blobID].position);
            }

            if (!transition || (transition && scene==ECLIPSE)) {
                // set position
                if (!(*vid).moveElement) {
                    (*vid).setDisplay( p, true);
                } else {
                    (*vid).goal.set( p );
                }
                // set opacity
                if (scene==ECLIPSE && segment==scenes[scene].segments-1) {
                    float d = ofDist(p.x, p.y, projectionW/2, projectionH/2);
                    (*vid).opMax = ((projectionW/2) - d) / (projectionW/2);
                }
            }
        }
        // update position of sparkly bridge
        if (!transition && scene==ATTRACTION) {

            // ofLogNotice("BRIDGE") << "\t\t" << ofGetFrameNum() << "\t" << "videoFollowBlob  bridge\t";
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
            if (scene==STARS) {
                // STARS: fade out frozen stars
                if (segment==SEG_STARS || !transition) {
                    (**it).fadeOut(0.001, 0.5, true);
                    ofLogNotice("BLOB") << "\t\t" << ofGetFrameNum() << "\t" << "\t\t\tunlinked stars and fadeOut ";
                } else {
                    if (segment==SEG_CONSTELLATIONS && transition) {
                        ofLogNotice("BLOB") << "\t\t" << ofGetFrameNum() << "\t" << "\t\t\tdon't unlink bc already transitioning ";
                    }
                }
                // if in transition, they are already fading out (and triggering allFaded Event)
            } else if (scene==ECLIPSE) {
                // (**it).fadeOut(0.05, 1.0, true);
                if (!(**it).fading) (**it).dead = true;
                ofLogNotice("BLOB") << "\t\t" << ofGetFrameNum() << "\t" << "\t\t\tunlinked planet and fadeOut ";
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
            string videoFile = "video/2_stars/LINK_0" + ofToString((**it).id) + "-outro-photoJPEG.mov";
            if ((**it).file != videoFile) {
                found = true;
                ofLogNotice("BRIDGE") << "\t" << ofGetFrameNum() << "\tunlinked bridge\t" << (**it).bridgeBlobID[0] << " " << (**it).bridgeBlobID[1];
                int oldW = (**it).w;
                int oldH = (**it).h;
                (**it).loadMovie(videoFile);
                (**it).w = oldW;
                (**it).h = oldH;
                (**it).reset(true);
                (**it).autoDestroy(true);
                numBridges--;
            }
        }
    }
    if (!found) ofLogNotice("BRIDGE") << "\t" << ofGetFrameNum() << "\t" << "\t\t\tbridge not found";
}

void planeApp::beginSegment() {
    ofLogNotice("TRANSITION") << "\t" << ofGetFrameNum() << "\t" << "beginSegment()   blobs-count = " << blobs.size();

    instructionTxt.fadeIn(instructionFadeIn);

    transition = false;

    // make new blob connections, to ensure blobs are connected to video elements.
    // exclude ECLIPSE planets, so that they don't change their look on every segment jump
    if (scene!=ECLIPSE || (scene==ECLIPSE && segment==0)) {
        for(std::map<int, Blob>::iterator it = blobs.begin(); it != blobs.end(); ++it){
            Blob* b = &it->second;
            ofNotifyEvent(b->onCreate,b->id,b);
        }
    }

}






// trigger specific outro transformations for segment / scene
//--------------------------------------------------------------
void planeApp::endSegment() {
    ofLogNotice("TRANSITION") << "\t" << ofGetFrameNum() << "\t" << "endSegment()   " << scene << ":" << segment;

    transition = true;

    // end instructions:
    if (scene==SUN && (segment==2 || segment==4)) {
        // FREEZE, put instruction outro video, but loop
        if (scenes[scene].instructionVid[language][segment][2] != "") {
            // outro video
            instructionVid->loadMovie(scenes[scene].instructionVid[language][segment][2]);
            instructionVid->loopFile = "";
            instructionVid->reset();
        }
        endedInstructions(scene);
    } else if (instructionVid->mediaLoaded) {
        // COME CLOSER, LET GO, EXHALE
        if (scenes[scene].instructionVid[language][segment][2] != "") {
            // outro video
            instructionVid->loadMovie(scenes[scene].instructionVid[language][segment][2]);
            instructionVid->loopFile = "";
            instructionVid->reset();
            instructionVid->finishMovie(1.0);
            ofAddListener( instructionVid->fadeOutEnd, this, &planeApp::endedInstructions );
        } else {
            if (scene==IDLE) {
                // fade out COME CLOSER
                instructionVid->fadeOut(0.04);
                ofAddListener( instructionVid->fadeOutEnd, this, &planeApp::endedInstructions );
            } else {
                // hard cut
                endedInstructions(scene);
            }
        }
    } else {
        // endedSegment = true;    // so that instructions are not displayed anymore
        // endedInstructions(scene);
        if (instructionTxt.rawText != "") {
            instructionTxt.fadeOut(instructionFadeOut);
            ofAddListener( instructionTxt.fadeOutEnd, this, &planeApp::endedInstructions );
        } else {
            // hard cut
            endedInstructions(scene);
        }
    }
}

void planeApp::endedInstructions(int & trans) {
    ofLogNotice("TRANSITION") << "\t" << ofGetFrameNum() << "\t" << "endedInstructions()";

    // better remove these
    if (!(scene==SUN && (segment==2 || segment==4))) {
        instructionVid->mediaLoaded = false;
    }
    ofRemoveListener( instructionVid->fadeOutEnd, this, &planeApp::endedInstructions );
    ofRemoveListener( instructionTxt.fadeOutEnd, this, &planeApp::endedInstructions );

    // default
    sceneChange = false;

    if (scene==IDLE) {                 // IDLE

        sceneChange = true;
        fgMediaFadedOut(scene);

    } else if (scene==STARS) {          // STARS

        if (segment<SEG_STARS) {
            moveOn = true;
        } else if (segment==SEG_STARS) {               // 0 - STAND STILL
            moveOn = true;
        } else if (segment==SEG_CONSTELLATIONS) {        // 1 - STAND STILL SOME MORE
            sceneChange = true;
            // fade out all stars and constellations, attach trigger
            fgMediaFadedOut(scene); 
        }

    } else if (scene==ATTRACTION) {     // ATTRACTION

        if (segment==0) {               // 0 - KEEP DISTANCE
            moveOn = true;
        } else if (segment==1) {        // 1 - KEEP DISTANCE
            sceneChange = true;
            // fade out all stars and bridges, no trigger
            int tmp = -1;
            fgMediaFadedOut(tmp); 

            // play starry outro video, with trigger
            fgMedia.push_back(attraction_outro);
            (*fgMedia[fgMedia.size()-1]).setDisplay(projectionW/2,projectionH/2, true);
            (*fgMedia[fgMedia.size()-1]).reset();
            (*fgMedia[fgMedia.size()-1]).autoDestroy(true);
            (*fgMedia[fgMedia.size()-1]).movieEndTrigger=true;
            ofAddListener( (*fgMedia[fgMedia.size()-1]).fadeOutEnd, this, &planeApp::allFaded );

            transition = true;
        }

    } else if (scene==REVOLUTIONS) {    // REVOLUTION

        if (segment==0) {               // 0 - SPIN
            // skip LET GO if there are no more planets on the screen
            if (planetCnt<=0) {
                // just in case
                int tmp = -1;
                fgMediaFadedOut(tmp); 

                ofLogNotice("TRANSITION") << "\t" << ofGetFrameNum() << "\t" << "endSegment()   planetCnt 0 ";
                segment+=2;
                sceneChange = true;
                bgMediaSwap(scene);     // trigger sun-intro video
            } else {
                moveOn = true;
                ofLogNotice("TRANSITION") << "\t" << ofGetFrameNum() << "\t" << "endSegment()   planetCnt " << planetCnt;
            }

        } else if (segment==1) {        // 1 - LET GO

            sceneChange = true;
            // fade out all planets, no trigger
            // int tmp = -1;
            // fgMediaFadedOut(tmp);

            for (vector<ofPtr<mediaElement> >::iterator it = fgMedia.begin(); it != fgMedia.end(); ++it) {
                if (!(**it).dead) {
                    if (!(**it).selfdestroy) {
                        (**it).loadMovie("video/3_revolution/REV_OUT-photoJPEG.mov");
                        (**it).reset(true);
                        (**it).autoDestroy(true);
                    } else {
                        ofLogNotice("TRANSITION") << "\t" << ofGetFrameNum() << "\t" << "endSegment()   media = selfdestroy";
                    }
                } else {
                    ofLogNotice("TRANSITION") << "\t" << ofGetFrameNum() << "\t" << "endSegment()   media = dead";
                }
            }

            bgMediaSwap(scene);   // trigger sun-intro video

        }

    } else if (scene==SUN) {            // SUN

        if (segment==0) {               // 0 - STAND ON ONE LEG
            moveOn = true;
        } else if (segment==1) {        // 1 - HOP
            moveOn = true;
        } else if (segment==2) {        // 2 - FREEZE

            if (success) {
                // GOT HERE BECAUSE: ALL FROZEN
                ofLogNotice("TRANSITION") << "\t" << ofGetFrameNum() << "\t" << "sunfreeze red";
                int randomRedFreeze = ofRandom(sun_freeze_red.size());
                fgMedia[0] = sun_freeze_red[randomRedFreeze];
                if ((*fgMedia[0]).mediaLoaded) {
                    (*fgMedia[0]).setDisplay(projectionW/2,projectionH/2, true);
                    (*fgMedia[0]).reset();
                    (*fgMedia[0]).finishMovie(1.0);
                    (*fgMedia[0]).movieEndTrigger=true;
                    ofAddListener( (*fgMedia[fgMedia.size()-1]).fadeOutEnd, this, &planeApp::allFaded );
                    ofLogNotice("TRANSITION") << "\t" << ofGetFrameNum() << "\t" << "play sun freeze red movie";
                    transition = true;
                } else {
                    moveOn = true;
                }
            } else {
                // GOT HERE BECAUSE TIME RAN OUT
                moveOn = true;
            }
        } else if (segment==3) {        // 3 - RUN EVERYWHERE
            // get rid of all blue surface videos, before advancing to next segment
            if (fgMedia.size()>1) {
                for (int i=1; i<fgMedia.size(); i++) {
                    fgMedia[i]->dead = true;
                }
            }
            // make sure yellow sun is displayed
            fgMedia[0]->hide = false;
            moveOn = true;
        } else if (segment==4) {        // 4 - FREEZE
            sceneChange = true;
            if (success) {
                // GOT HERE BECAUSE: ALL FROZEN
                ofLogNotice("TRANSITION") << "\t" << ofGetFrameNum() << "\t" << "sunfreeze red";
                int randomRedFreeze = ofRandom(sun_freeze_red.size());
                fgMedia[0] = sun_freeze_red[randomRedFreeze];
                if ((*fgMedia[0]).mediaLoaded) {
                    (*fgMedia[0]).setDisplay(projectionW/2,projectionH/2, true);
                    (*fgMedia[0]).reset();
                    (*fgMedia[0]).autoDestroy(true);
                    (*fgMedia[0]).movieEndTrigger=true;
                    ofAddListener( (*fgMedia[fgMedia.size()-1]).fadeOutEnd, this, &planeApp::allFaded );
                    transition = true;
                } else {
                    moveOn = true;
                }
            } else {
                // GOT HERE BECAUSE TIME RAN OUT
                instructionVid->mediaLoaded = false;
                (*fgMedia[0]).fadeOut();
                ofAddListener( (*fgMedia[fgMedia.size()-1]).fadeOutEnd, this, &planeApp::allFaded );
                // moveOn = true;
            }
        }

    } else if (scene==ECLIPSE) {        // ALIGNMENT

        if (segment==0) {               // 0 - LINE UP IN FRONT OF ME
            moveOn = true;
        } else if (segment==1) {        // 1 - FOLLOW ME
            moveOn = true;
        } else if (segment==2) {        // 2 - 
            moveOn = true;
        } else if (segment==3) {        // 3 - 
            moveOn = true;
        } else if (segment==4) {        // 4 - 
            moveOn = true;
        } else if (segment==5) {        // 5 - 
            moveOn = true;
        } else if (segment==6) {        // 6 
            moveOn = true;
        } else if (segment==7) {        // 7 - DISPERSE SLOWLY
            sceneChange = true;
            // fade out all planets, add trigger
            (*fgMedia[0]).fadeOut(0.01, 1.0, true);
            ofAddListener( (*fgMedia[0]).fadeOutEnd, this, &planeApp::allFaded );
            // fgMediaFadedOut(scene);
        }

    } else if (scene==SHOOTING) {       // SHOOTING STARS

        if (segment==0) {               // 0 - MOVE LIKE A SHOOTING STAR
            moveOn = true;
        } else if (segment==1) {        // 1 - DROP
            moveOn = true;
        } else if (segment==2) {        // 2 - EXHALE
            // no need to fade out all shooting stars
            moveOn = true;
        } else if (segment==3) {        // 3 - STAND UP
            moveOn = true;
        } else if (segment==4) {        // TITLE SEQUENCE
            sceneChange = true;
            moveOn = true;
        }

    }

    if (sceneChange) transition = true;

}


void planeApp::jumpToScene(int s) {
    ofLogNotice("TRANSITION") << "\t" << ofGetFrameNum() << "\t" << "jump to scene " << s;
    int oldScene = scene;
    instructionTxt.opacity = 0.0;
    scene = s;
    segment = 0;
    sceneChange = true;
    transition = true;
    initSegment();
    int tmp = -2;
    if (scene==IDLE) bgMediaSwap(scene);
    else if (oldScene==0 && scene>0) bgMediaSwap(tmp);
}

// decide what the next segment / scene will be
//--------------------------------------------------------------
void planeApp::nextSegment() {
    ofLogNotice("TRANSITION") << "\t" << ofGetFrameNum() << "\t" << "nextSegment";
    segment++;

    // MOVE ON TO NEW SCENE
    if(scene == -1 || segment >= scenes[scene].segments) {
        scene++;
        segment = 0;
        if(scene >= scenes.size()) {
            // terminate program
            ofLogNotice("TRANSITION") << "\t" << ofGetFrameNum() << "\t" << "reached end, force terminate program, so it restarts itself again";
            ofExit();
        }
    }

    // SKIP SEGMENTS
    if (scene==ATTRACTION && segment>1) {
        // skip if only 1 blob
        if (blobsOnStage<2) {
            scene++;
            segment = 0;
            sceneChange = true;
        }
    }

    // IF NO ACTIVITY, GO TO IDLE MODE
    if (lastActivity > inactivityTimer && autoplay) {
        if (ofGetElapsedTimef() > 1800) {
            // terminate program, every half hour, for a fresh start
            ofLogNotice("TRANSITION") << "\t" << ofGetFrameNum() << "\t" << "30min idle, force terminate program, so it restarts itself again";
            ofExit();
        } else {
            ofLogNotice("TRANSITION") << "\t" << ofGetFrameNum() << "\t" << "no activity since " << lastActivity << "sec, go to IDLE";
            scene = IDLE;
            segment = 0;
            sceneChange = true;
            languageChange();            
        }

    }

    if (scene==IDLE) {
        bgMediaSwap(scene);
        globalStart = ofGetUnixTime();
    }

    if (sceneChange)
        ofLogNotice("TRANSITION") << "\t" << ofGetFrameNum() << "\t" << "sceneChange! scene " << scene << ": " << scenes[scene].name;
    ofLogNotice("TRANSITION") << "\t" << ofGetFrameNum() << "\t" << "---------------------------\t" << scene << " : " << segment << "\t" << ofGetFrameRate() << "FPS\t" << ofGetElapsedTimef() << "sec";

    initSegment();

}

void planeApp::initSegment(){

    ofLogNotice("TRANSITION") << "\t" << ofGetFrameNum() << "\t" << "initSegment()";

    guiScene = scene;
    guiSegment = segment;
    stateGui.saveToFile("currentState.xml");


    configureBlobserver();

    if (sceneChange || scene==0) {
        ofLogNotice("TRANSITION") << "\t" << ofGetFrameNum() << "\t\t" << "cleared " << fgMedia.size() << " fgMedia videos";
        fgMedia.clear();   // delete all foreground videos
    }
    success = false;
    successCnt = 0;
    activityCnt = 0;
    followMe = -0.5;
    if (segment==0) planetCnt = 0;
    // flash = true;       //
    segmentStart = ofGetUnixTime();
    segmentClock = 0;

    string instruction = scenes[scene].instructions[language][segment];
    string measureInst = instruction;
    // hack to allow for french-accented letters to produce correct center-align text-block width
    if(language==1) measureInst = scenes[scene].instructions[2][segment];
    instructionTxt.setText(instruction, measureInst);
    instructionTxt.opacity = 0.f;


    // INSTRUCTION IMAGE, if present
    // string in_img = scenes[scene].instructionImg[language][segment];
    // if (in_img.length() > 2) {
    //     instructionImg.loadImage("img/" + in_img);
    //     // instructionImg.loadImage("img/placeholder_stop.jpg");
    //     ofLogNotice("TRANSITION") << "\t" << ofGetFrameNum() << "\t" << "load instruction image " << in_img;
    // }

    // INSTRUCTION VIDEO, if present
    string in_vid1 = scenes[scene].instructionVid[language][segment][0]; // intro video
    string in_vid2 = scenes[scene].instructionVid[language][segment][1]; // loop video
    if (in_vid1.length() > 2) {
        // intro video
        instructionVid->loadMovie(in_vid1);
        instructionVid->reset();
        instructionVid->loopFile = in_vid2;
        instructionVid->finishMovie(1.0);
    } else if (in_vid2.length() > 2) {
        // no intro video, but a loop video
        instructionVid->loadMovie(in_vid2);
        instructionVid->reset();
    } else {
        // no instruction video
        instructionVid->mediaLoaded = false;
    }
    instructionVid->setDisplay(projectionW/2,projectionH/2, true);

    


    // SET BG STARRY-SKY OPACITY LOWER FOR STARS
    if (scene==STARS && segment>=SEG_STARS) (*bgMedia[5]).opMax = 0.35;
    else (*bgMedia[5]).opMax = 1;

    // add FG videos
    if (scene==STARS) {
        if (segment==0) {
            fgMedia.push_back(ofPtr<mediaElement>( title_sequence[language] ));
            (*fgMedia[fgMedia.size()-1]).setDisplay(projectionW/2,projectionH/2, true);
            (*fgMedia[fgMedia.size()-1]).reset();
            (*fgMedia[fgMedia.size()-1]).autoDestroy(true);
            (*fgMedia[fgMedia.size()-1]).finishMovie(1.0);
            ofAddListener( (*fgMedia[fgMedia.size()-1]).fadeOutEnd, this, &planeApp::allFaded );
        } else if (segment==1) {
            fgMedia.push_back(ofPtr<mediaElement>( diagram_sequence[language] ));
            (*fgMedia[fgMedia.size()-1]).setDisplay(projectionW/2,projectionH/2, true);
            (*fgMedia[fgMedia.size()-1]).reset();
            (*fgMedia[fgMedia.size()-1]).autoDestroy(true);
            (*fgMedia[fgMedia.size()-1]).finishMovie(1.0);
            ofAddListener( (*fgMedia[fgMedia.size()-1]).fadeOutEnd, this, &planeApp::allFaded );
        } 
    } else if (scene==REVOLUTIONS) {

    } else if (scene==SUN) {
        // SUN, load sun as fgMedia
        if (sceneChange) {

            // fgMedia.push_back(ofPtr<mediaElement>( new videoElement("video/4_sun/SUN_stand_jump-loop_4-qtPNG.mov",false)));
            fgMedia.push_back(ofPtr<mediaElement>( sun_jump ));
            (*fgMedia[fgMedia.size()-1]).setDisplay(projectionW/2,projectionH/2, true);
            (*fgMedia[fgMedia.size()-1]).reset();
        }
        if (segment==2) {
            // (*fgMedia[0]).loadMovie("video/4_sun/SUN_run_loop-1-qtPNG.mov");
            fgMedia[0] = sun_run;
            (*fgMedia[0]).setDisplay(projectionW/2,projectionH/2, true);
            (*fgMedia[0]).reset();
            // (*fgMedia[0]).dead = true;
            // fgMedia.push_back(ofPtr<mediaElement>( new videoElement("video/4_sun/SUN_run_loop-1-qtPNG.mov",false)));
            // (*fgMedia[fgMedia.size()-1]).setDisplay(projectionW/2,projectionH/2, true);
            // (*fgMedia[fgMedia.size()-1]).reset();
        } else if (segment==3) {
            // RUN EVERYWHERE
            // (*fgMedia[0]).loadMovie("video/4_sun/SUN_run_loop-1-qtPNG.mov");
            fgMedia[0] = sun_run;
            (*fgMedia[0]).setDisplay(projectionW/2,projectionH/2, true);
            (*fgMedia[0]).reset();
        }

    } else if (scene==ECLIPSE) {

        if (sceneChange) {
            // ECLIPSE. create white/black MAIN PLANET = fgMedia[0]
            fgMedia.push_back(ofPtr<mediaElement>( new imageElement("video/5_eclipse/WHITE_PLANET.png")));
            (*fgMedia[fgMedia.size()-1]).setDisplay( projectionW/2 + moonPosX, projectionH/2, true );
            (*fgMedia[fgMedia.size()-1]).reset();
            (*fgMedia[fgMedia.size()-1]).opMax = 0.8;
            (*fgMedia[fgMedia.size()-1]).fadeIn(0.005);
            // (*fgMedia[fgMedia.size()-1]).fadeTo(0.8);
            // create ALIGNMENT GLOW, loop and hide   = fgMedia[1]
            string alignmentGlow = "video/5_eclipse/LIGHT-photoJPEG.mov";
            fgMedia.push_back(ofPtr<mediaElement>( new videoElement(alignmentGlow, true) ));
            (*fgMedia[fgMedia.size()-1]).setDisplay((*fgMedia[0]).position.x, (*fgMedia[0]).position.y, true);
            (*fgMedia[fgMedia.size()-1]).reset(true);
            (*fgMedia[fgMedia.size()-1]).opacity = 0;
        }
        if (segment==2) {
            // just to make sure moon and glow are back to center position
            (*fgMedia[0]).setDisplay( projectionW/2 + moonPosX, projectionH/2, true );
            (*fgMedia[1]).position.x = (*fgMedia[0]).position.x;
        } else if (segment==scenes[scene].segments-1) {
            // fade out white planet, and hide glow
            //(*fgMedia[0]).fadeOut(0.01, 1.0, true);
            (*fgMedia[1]).hide = true;
        }
    } else if (scene==SHOOTING) {
        if (sceneChange) shootingPointer = 0;
        if (segment==scenes[scene].segments-1) {
            // last scene, TITLE SEQUENCE
            fgMedia.push_back(ofPtr<mediaElement>( title_sequence[language] ));
            (*fgMedia[fgMedia.size()-1]).setDisplay(projectionW/2,projectionH/2, true);
            (*fgMedia[fgMedia.size()-1]).reset();
            (*fgMedia[fgMedia.size()-1]).autoDestroy(true);
            (*fgMedia[fgMedia.size()-1]).finishMovie(1.0);
            ofAddListener( (*fgMedia[fgMedia.size()-1]).fadeOutEnd, this, &planeApp::allFaded );
        }
    }

    beginSegment();

}

// check for incoming OSC messages
// add new blobs, or update blobs with new data
//--------------------------------------------------------------
void planeApp::receiveOsc(){

    // check for waiting messages
	while (receiver.hasWaitingMessages()) {

        if (!oscActive) oscActive = true;
        oscLastMsg = ofGetElapsedTimef();

		// get the next message
		ofxOscMessage m;
		receiver.getNextMessage(&m);

		if(m.getAddress() == "/blobserver/startFrame"){

            currentFlowId = m.getArgAsInt32(1);
            if (currentFlowId==hogFlowId) 
                ofLogNotice("OSC") << "\t\t" << ofGetFrameNum() << "\t" << "/blobserver/startFrame " << m.getArgAsInt32(0);

        } else if(m.getAddress() == "/blobserver/endFrame"){

            currentFlowId = m.getArgAsInt32(1);
            if (currentFlowId==hogFlowId) {
                oscMsgReceived = true;  // ready to process blob data
                ofLogNotice("OSC") << "\t\t" << ofGetFrameNum() << "\t" << "/blobserver/endFrame";
            }

		} else if(m.getAddress() == "/blobserver/bgsubtractor"){
            // ignore
		} else if(m.getAddress() == "/blobserver/stitch"){
            // ignore
        } else if(m.getAddress() == "/blobserver/nop"){
            // ignore
		} else if(m.getAddress() == "/blobserver/broadcast"){

            string var = m.getArgAsString(0);
            string source = m.getArgAsString(1);
            string cam = m.getArgAsString(2);
            float val = ofToFloat( m.getArgAsString(3) );
            if (var.compare("exposureTime") == 0) cameraExposure = val;
            ofLogNotice("OSC") << "BROADCAST " << var << ", " << source << ", " << cam << ", " << val;

		} else if(m.getAddress() == "/blobserver/hog" && currentFlowId==hogFlowId) {

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
            // multiply xy so that blobposition represents foot-position and not x/y of tracking window
            b->setRawPosition(posx + blobW*0.95, posy + blobH*0.95);
            // b->follow(posx + blobW*0.95, posy + blobH*0.95, siteW, siteH, stageRadius, y_mean);
            b->setVelocity(velx, vely);
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

    ofBackground(0);

    if (ofGetFrameNum() > 10 ) {

        // ofBackground(0);
        int offsx = 500;
        if(!projectorOn) offsx = 10;
        int offsy = 10;

        // ofFill(); ofSetColor(255);

        if (drawDebugScreen) this->drawRawData(offsx, offsy, 0.45, true);

        offsy += 260 + 10;
        if (drawDebugScreen) this->drawTopDown(offsx, offsy, 0.5, drawBlobDetail);

        offsy = 10;
        offsx += 280;
        if (drawDebugScreen) this->drawAnalysis(offsx, offsy, 0.35);

        offsy = 10;
        offsx += 370;

        if (drawDebugScreen) this->drawScreen(offsx, offsy, 0.35);


        offsx += 430;
        this->drawControlInfo(offsx, offsy);

        stateGui.draw();
        gui.draw();

        // MAIN VISUALS SCREEN
        ofPushMatrix();
        ofTranslate(0,2100);
        ofRotateZ(-90);
        this->drawScreen(0, 0, 1);
        ofPopMatrix();
    }
}


//--------------------------------------------------------------
void planeApp::drawScreen(int x, int y, float scale){

    ofPushMatrix();
    ofTranslate(x, y);
    ofScale(scale, scale, scale);

    // reset these values for now, before cleaning them out of all the following code
    x = 0;
    y = 0;
    scale = 1.f;

    // black frame first
    ofFill(); ofSetColor(0);
    ofRect(0,0,projectionW,projectionH);
    ofNoFill(); ofSetColor(255);

    // BACKGROUND VIDEOS
    ofEnableBlendMode(OF_BLENDMODE_ADD);
    nebula->draw(projectionOffsetX,0,1);
    (*bgMedia[bgMediaId]).draw(projectionOffsetX,0,1);
    ofDisableBlendMode();


    // if (ofGetFrameRate()>10) {
    // foreground videos, without BLENDING mode
    ofEnableAlphaBlending();
    for (vector<ofPtr<mediaElement> >::iterator it = fgMedia.begin(); it != fgMedia.end(); ++it) {
        if (!(**it).dead && !(**it).blend) (**it).draw(0, 0, 1);
    }
    ofDisableAlphaBlending();

    // FOREGROUND VIDEOS
    ofEnableBlendMode(OF_BLENDMODE_ADD);
    for (vector<ofPtr<mediaElement> >::iterator it = fgMedia.begin(); it != fgMedia.end(); ++it) {
        if (!(**it).dead && (**it).blend) (**it).draw(0, 0, 1);   //
    }
    ofDisableBlendMode();
    // }

    // INSTRUCTION VIDEOS
    if (instructionVid->mediaLoaded) {
        ofEnableBlendMode(OF_BLENDMODE_ADD);
        instructionVid->draw(0,0,1);
        ofDisableBlendMode();
    } 

    // INSTRUCTION TEXT
    if (scenes[scene].instructionVid[language][segment][0].length() < 2 && scenes[scene].instructionVid[language][segment][1].length() < 2) {
        ofFill();
        if (scene==ECLIPSE && segment==SEG_FOLLOWME) {
            // FOLLOW ME
            instructionTxt.draw(&fontBg, (*fgMedia[0]).position.x - moonPosX, 1742);
        } else if (scene==IDLE) {
            // IDLE MODE = INSTRUCTION TEXT PULSES
            int animCnt = ofGetFrameNum()%120;
            float dimAlpha = 0.3;
            float instructionTxtAlpha = 1.;
            if (animCnt < 30) {
                instructionTxtAlpha = dimAlpha + (animCnt/30.f)*(1-dimAlpha);
            } else if (animCnt > 90) {
                instructionTxtAlpha = 1.0 - ((animCnt-90)/30.f)*(1-dimAlpha);
            } else {
                instructionTxtAlpha = 1.;
            }
            instructionTxt.opMax = instructionTxtAlpha;
            instructionTxt.draw(&fontIdle, projectionW/2, 1742);
        } else {
            instructionTxt.draw(&fontBg, projectionW/2, 1742);
        }
    }

    // INSTRUCTION IMAGES, display always, so that they are visible on transition videos (red sun)
    // if (in_img.length() > 2) {
    //     ofEnableBlendMode(OF_BLENDMODE_ADD);
    //     ofFill();
    //     ofSetColor(255,255,255);
    //     instructionImg.draw(0,0, projectionW, projectionH);

    //     ofDisableBlendMode();
    // }

    // extra debug information
    if (displayDebug) {
        // fontSm.drawString(scenes[scene].analysis[segment], 100, (projectionH-150));
        fontBg.drawString(ofToString(segmentClock), (projectionW-200), (projectionH-200));
        fontSm.drawString(ofToString(success ? "true" : "false"), (projectionW-200), (projectionH-150));
        this->drawRawData(330, projectionH-200, -0.5, false);
    }

    ofPopMatrix();
}

//--------------------------------------------------------------
void planeApp::drawAnalysis(int x, int y, float scale){

    // frame first
    ofFill(); ofSetColor(30,30,50);
    ofRect(x,y,1000*scale,projectionH*scale);
    ofNoFill(); ofSetColor(255);
    ofRect(x,y,1000*scale,projectionH*scale);

    string analysisTxt = scenes[scene].analysis[segment];
    ofFill(); ofSetColor(255);
    fontSm.drawString(analysisTxt, x+20, y + projectionH*scale*0.05);


    if (scene==IDLE) {
        // IDLE mode
        // draw circle for each blob
        int bx = x + 100; int by = y + 150;
        ofFill();
        for(std::map<int, Blob>::iterator it = blobs.begin(); it != blobs.end(); ++it){
            Blob* b = &it->second;
            if (b->onStage && by < projectionH*scale-100) {
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
    } else if (scene==STARS) {
        // STAND STILL
        // draw circle for each blob, and display frozen, frozentimer
        int bx = x + 80; int by = y + 150;
        ofFill();
        for(std::map<int, Blob>::iterator it = blobs.begin(); it != blobs.end(); ++it){
            Blob* b = &it->second;
            if (b->onStage && by < projectionH*scale-100) {
                if(b->properFreeze) {
                    if(segment==SEG_CONSTELLATIONS && b->frozenTimer > freezeMaxTime) {
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

    } else if (scene==ATTRACTION) {

        // KEEP THE DISTANCE
        int bx = x + 80; int by = y + 150;
        ofFill();
        for(std::map<int, Blob>::iterator it = blobs.begin(); it != blobs.end(); ++it){
            Blob* b = &it->second;
            if (b->onStage && by < projectionH*scale-100) {
                if (b->movingMean) ofSetColor(255);
                else ofSetColor(100);

                ofCircle(bx, by, 50);
                ofSetColor(0,0,255);
                ofDrawBitmapString( ofToString(b->id, 4, '0'), bx-15, by+5);

                string textStr = "neighbors: "+ ofToString(b->neighbors.size());
                textStr += "\nmovingMean: " + ofToString(b->movingMean ? "true" : "false");
                textStr += "\nvel \t: "+ ofToString(b->vel,2);
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
    } else if (scene==SUN || scene==ECLIPSE) {

        // HOP
        int bx = x + 80; int by = y + 150;
        ofFill();
        for(std::map<int, Blob>::iterator it = blobs.begin(); it != blobs.end(); ++it){
            Blob* b = &it->second;
            if (b->onStage && by < projectionH*scale-100) {

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

    } else if (scene==SHOOTING) {
        // SHOOTING STARS
        int bx = x + 80; int by = y + 150;
        ofFill();
        for(std::map<int, Blob>::iterator it = blobs.begin(); it != blobs.end(); ++it){
            Blob* b = &it->second;
            if (b->onStage && by < projectionH*scale-100) {

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
    ofDrawBitmapString("FPS\n--------------\n" + ofToString(ofGetFrameRate()) +
                       "\n\nSCENE:SEGMENT\n" + "--------------\n" + ofToString(scene) + ":" + ofToString(segment) +
                       "\n\nSEGM LENGTH\n" + "--------------\n"  + ofToString(scenes[scene].length[segment]) +
                       "\n\nSEGMENT TIME\n" + "--------------\n"  + ofToString(segmentClock) +
                       "\n\nGLOBAL TIME\n" + "--------------\n"  + ofToString(masterClock) +
                       "\n\nLAST ACTIVITY\n" + "--------------\n"  + ofToString(lastActivity) +


                       "\n\nTRANSITION\n" + "--------------\n"  + ofToString(transition ? "true" : "false") +
                       "\n\nMOVEON\n" + "--------------\n"  + ofToString(moveOn ? "true" : "false") +
                       "\n\nSUCCESS\n" + "--------------\n"  + ofToString(success ? "true" : "false") +
                       "\n\nSUCCESS CNT\n" + "--------------\n"  + ofToString(successCnt) +
                       "\nACTIVITY CNT\n" + "--------------\n"  + ofToString(activityCnt) +
                       "\nPLANET CNT\n" + "--------------\n"  + ofToString(planetCnt) +
                       "\nBRIDGES\n" + "--------------\n"  + ofToString(numBridges) +
                       "\nFG MEDIA\n" + "--------------\n"  + ofToString(fgMedia.size()), x+3, y+10 );
}


// draw raw data / small display
//--------------------------------------------------------------
void planeApp::drawRawData(int x, int y, float scale, bool displayText){

//    int blobserverW = 192;
//    int blobserverH = 144;

    // frame first
    ofFill(); ofSetColor(30,30,50);
    ofRect(x,y,blobserverW*scale,blobserverH*abs(scale));
    ofNoFill(); ofSetColor(255);
    ofRect(x,y,blobserverW*scale,blobserverH*abs(scale));

    if (displayText) {
        // write information
        string rawInfo = "port: \t\t" + ofToString(MYPORT);
        rawInfo += "\nnetwork ready:\t" + ofToString(oscNetworkReady ? "true" : "false");
        rawInfo += "\nBLOBSERVER: \t" + ofToString(BLOBSERVERIP); //  + " (" + ofToString(BLOBPORT) + ")";
        rawInfo += "\nosc active: \t" + ofToString(oscActive ? "true" : "false");
        rawInfo += "\nosc last msg: \t" + ofToString(oscLastMsgTimer,2) + " sec";
        rawInfo += "\nexposure: \t" + ofToString(cameraExposure,5);
        rawInfo += "\nhog velocity: \t" + ofToString(hogAvVel,2);
        ofDrawBitmapStringHighlight(rawInfo, x + 3, y + blobserverH*scale + 15);
    }

    // draw frame for each blob. blobserver frame size = 64 x 128 px
    for(std::map<int, Blob>::iterator it = blobs.begin(); it != blobs.end(); ++it){
        Blob* b = &it->second;
        ofRect( x + b->_rawPos.x*scale - blobW*scale/2.0, y + b->_rawPos.y*abs(scale) - blobH*abs(scale)*0.95, blobW*scale, blobH*abs(scale));
        ofCircle(x + b->_rawPos.x*scale, y + b->_rawPos.y*abs(scale), 3*scale, 3*abs(scale));
    }

    // draw steles
    for (int i=0; i<7; i++) {
        if (i==0) ofCircle(x + steles[i].x*scale, y + steles[i].y*abs(scale), 5*scale, 5*abs(scale));
        else ofCircle(x + steles[i].x*scale, y + steles[i].y*abs(scale), 10*scale, 10*abs(scale));
    }

}

//--------------------------------------------------------------
void planeApp::drawTopDown(int x, int y, float scale, bool detailed) {

    // frame
    ofFill(); ofSetColor(30,30,50);
    ofRect(x,y,siteW*scale,siteH*scale);
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
    for (int i=0; i<7; i++) {
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

    if (oscNetworkReady && configBlobserver) {

        ofLogNotice("TRANSITION") << "\t" << ofGetFrameNum() << "\t" << "configureBlobserver()";

        // standard setting:
        bool erratic = false;  // else slow movements
        bool allowLessFP = false;        // allow more false positives
        bool slow = false;              // extra slow?

        bool mergeFar = true;

        if (scene==IDLE) {                 // IDLE

        } else if (scene==STARS) {          // STARS
            slow = true;

            if (segment==0) {               // STAND STILL
            } else if (segment==1) {        // STAND STILL SOME MORE
            }

        } else if (scene==ATTRACTION) {          // STARS
            slow = true;

        } else if (scene==REVOLUTIONS) {          // REVOLUTION
            mergeFar = false;

            if (segment==0) {               // SPIN
                erratic = true;
            } else if (segment==1) {        // LET GO
                erratic = true;
            }

        } else if (scene==SUN) {          // SUN

            if (segment==0) {               // STAND ON ONE LEG
            } else if (segment==1) {        // HOP
                erratic = true;
                allowLessFP = true;
            } else if (segment==2) {        // FREEZE
            } else if (segment==3) {        // RUN EVERYWHERE
            } else if (segment==4) {        // FREEZE
            }

        } else if (scene==ECLIPSE) {          // ALIGNMENT

            slow = true;

            if (segment==0) {               // LINE UP IN FRONT OF ME
            } else if (segment==1) {        // FOLLOW ME
            } else if (segment==2) {        // STEP OUT OF THE LINE
            } else if (segment==3) {        // STEP INTO THE LINE
            } else if (segment==4) {        // STEP OUT OF THE LINE
            } else if (segment==5) {        // STEP INTO THE LINE
            } else if (segment==6) {        // DISPERSE SLOWLY
            }

        } else if (scene==SHOOTING) {          // SHOOTING STARS
            // erratic = true;
            // allowLessFP = true;
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


        sendOscMsgToHog("setParameter", "mergeDistance", (mergeFar) ? 60.0 : 52.0);


        if (allowLessFP) {
            // ALLOW FOR LESS FALSE POSITIVES (jumping)
            // sendOscMsgToHog("setParameter", "margin", 0.3);
        } else {
            // NORMAL SETTING : MARGIN
            sendOscMsgToHog("setParameter", "margin", 0.0);
        }
    }

}

void planeApp::languageChange() {
    languageCnt = (languageCnt+1>languageRatio) ? 0 : languageCnt+1;
    language = (languageCnt==0) ? 0 : 1;
    ofLogNotice("TRANSITION") << "\t" << ofGetFrameNum() << "\t" << "change language " << language;
    stateGui.saveToFile("currentState.xml");
}


void planeApp::exit() {
    // do some destructing here
    // sendOscMsg("signOut", MYIP); // don't send disconnect, else blobserver terminates!!
    ofLogNotice() << "\t\t\t" << ofGetFrameNum() << "\t" << "goodbye";
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

    if (key == 'm') {
        // drawBlobDetail = !drawBlobDetail;
        // cout << "print debug info" << endl;
        // printDebugInfo();
        
        drawDebugScreen = !drawDebugScreen;
    }
    if (key =='d') {
        displayDebug = !displayDebug;
    }
    if (key == 'b') {
        drawBridge = !drawBridge;
    }

    if (key == OF_KEY_RETURN) {
        processing = !processing;
    }

    if (key == OF_KEY_RIGHT){
        if (!transition) {
            ofLogNotice("KEY") << "\t\t\t" << ofGetFrameNum() << "\t" << "==============>" << key << " end segment ";
            endSegment();
        }
    }
    if (key == ' ') {
        autoplay = !autoplay;
    }
    // if (key == 't') {
    //     testMode = !testMode;
    // }
    if(key == 's') {
		gui.saveToFile("planets01.xml");
        stateGui.saveToFile("currentState.xml");
	}
	if(key == 'l') {
		gui.loadFromFile("planets01.xml");
        stateGui.loadFromFile("currentState.xml");
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
    if (key=='u') {
        languageChange();
    }

    if (key=='p') {
        ofLogNotice() << "make screenshot";
        ofImage img;
        img.grabScreen(0,0,1425,700);
        string fileName = "plane_"+ofGetTimestampString()+".png";
        img.saveImage(fileName);
        ofLogNotice() << "saved screenshot " << fileName.c_str();
    }

    if (key>='0' && key<='6') {
        int s = int(key) - int('0');
        ofLogNotice("KEY") << "\t\t\t" << ofGetFrameNum() << "\t" << "==============>" << key << " scene = " << s;
        jumpToScene(s);
    }

    if (key=='g' && scene==ECLIPSE) {
        // FAKE ALIGNMENT GLOW
        // fgMedia[1]->hide = !fgMedia[1]->hide;
        ofLogNotice("KEY") << "\t\t\t" << ofGetFrameNum() << "\t" << "==============>" << key << " fgMedia[1].opacity = " << fgMedia[1]->opacity;
        if (fgMedia[1]->opacity < 0.5) {
            fgMedia[1]->fadeOut(-0.1, fgMedia[1]->opacity, false);
        } else {
            fgMedia[1]->fadeOut(0.1, fgMedia[1]->opacity, false);
        }
    }

    if (key=='r' && scene==SHOOTING) {
        // rain shooting stars
        float randdeg = ofRandom(-5.f, 5.f);
        for (int i=0; i<10; i++) {
            if (++shootingPointer>=shooting_stars.size()) shootingPointer=0;
            fgMedia.push_back(shooting_stars[shootingPointer]);
            (*fgMedia[fgMedia.size()-1]).setDisplay(ofRandom(projectionW-100), ofRandom(projectionH-100), true);
            (*fgMedia[fgMedia.size()-1]).reset();
            (*fgMedia[fgMedia.size()-1]).moveAcross( randdeg, 45.f, projectionW, false);
            (*fgMedia[fgMedia.size()-1]).autoDestroy(true);
            (*fgMedia[fgMedia.size()-1]).movie->setSpeed(0.5);
        }
    }

    if (key=='e' && scene==SUN) {
        // blue explosion]
        ofLogNotice("KEY") << "\t\t\t" << ofGetFrameNum() << "\t" << "==============>" << key << " blue explosion";

        int randomExpl = ofRandom(7) + 1;
        string videoEnd = "_fullscale-blue-posterized-centered-qtPNG.mov";
        string newVideoName = "video/4_sun/SUN_explosion-" + ofToString(randomExpl,2,'0') + videoEnd;
        fgMedia.push_back(ofPtr<mediaElement>( new videoElement(newVideoName,false)));
        (*fgMedia[fgMedia.size()-1]).setDisplay(projectionW/2 + ofRandom(-300,300), projectionH/2 + ofRandom(-300,300), true);
        (*fgMedia[fgMedia.size()-1]).reset();
        (*fgMedia[fgMedia.size()-1]).autoDestroy(true);
    }

    if (key=='k' && scene==REVOLUTIONS) {

        ofLogNotice("KEY") << "\t\t\t" << ofGetFrameNum() << "\t" << "==============>" << key << " add revolution";
        // add planet
        if (planetCnt<5) {
            planetCnt++;
            string videoFile;
            int videoPick = ofRandom(5) + 1;
            videoFile = "video/3_revolution/REV_0"+ofToString(videoPick)+"-photoJPEG.mov";
            fgMedia.push_back(ofPtr<mediaElement>( new videoElement(videoFile,true)));
            (*fgMedia[fgMedia.size()-1]).reset(true);
            positionRevolutions();
        }
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
