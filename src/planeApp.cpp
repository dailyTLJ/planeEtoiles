#include "planeApp.h"
#include "ofBitmapFont.h"

//--------------------------------------------------------------
void planeApp::setup(){

    //old OF default is 96 - but this results in fonts looking
    // larger than in other programs.
	ofTrueTypeFont::setGlobalDpi(72);

    font.loadFont("Mercury Bold.otf", 36, true, true);
	font.setLineHeight(30.0f);
	font.setLetterSpacing(1.037);

	mouseX = 0;
	mouseY = 0;
	mouseButtonState = "";

    autoplay = false;
	drawBlobDetail = true;

	gui.setup("panel", "settings.xml", 800,400);
	gui.add(freezeMinVel.set( "Freeze Minimum Velocity",0.1, 0, 5.0 ));
	gui.add(freezeMinTime.set( "Freeze Minimum Time",0, 0, 30 ));
	gui.add(freezeMaxTime.set( "Freeze Maximum Time",10, 0, 30 ));

    // init
	this->initScenes();

    // compute the perspectiveTransformation
    // to map from blob-coordinates to the top-down view
    float in[] = {0,0,150,0,150,30,0,30};
    float out[] = {0,0,640,0,590,480,50,480};
    cv::Mat proj_in(4,2, CV_32FC1, in);
    cv::Mat proj_out(4,2, CV_32FC1, out);
    perspectiveMat = cv::findHomography(proj_in, proj_out);
    proj_in.release();
    proj_out.release();

    // test blob to verify perspective Mat
    testBlob.perspectiveMat = &perspectiveMat;
    for( int i=0; i<50; i++ ) {
        testBlob.follow( i*3, i * 3.0/5.0 );
        testBlob.follow(150-i*3,i * 3.0/5.0);
    }


    // listen on the given port
	cout << "listening for osc messages on port " << PORT << "\n";
	receiver.setup(PORT);

	ofBackground(255);

}

//--------------------------------------------------------------
void planeApp::initScenes(){

    globalStart = ofGetUnixTime();
    segmentStart = ofGetUnixTime();
    scene = 0;
	segment = 0;
	segmentClock = 0;
	masterClock = 0;

    sceneInfo idle;
    idle.name = "Idle";
    idle.no = 0;
    idle.segments = 1;
    idle.instructions[0] = "Come closer!";
    idle.length[0] = -1;
    scenes[0] = idle;

    sceneInfo stars;
    stars.name = "Star Links";
    stars.no = 1;
    stars.segments = 4;
    stars.instructions[0] = "Stand still.";
    stars.length[0] = 20;
    stars.instructions[1] = "Try new spots \nto light up \nmore stars.";
    stars.length[1] = 20;
    stars.instructions[2] = "Walk with \nsomeone. Keep the \nsame distance \nbetween you.";
    stars.length[2] = 30;
    stars.instructions[3] = "Walk with \nsomeone. Make eye \ncontact. Keep \nthe distance.";
    stars.length[3] = 30;
    scenes[1] = stars;

}

//--------------------------------------------------------------
void planeApp::update(){

    this->receiveOsc();

	// check for dead blobs, and delete them from list
    std::map<int,Blob>::iterator it = blobs.begin();
    while (it != blobs.end()) {
    	it->second.update();
    	if( !it->second.isAlive() ) {
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
    if (scene==0) {
        if (segment==0) {

        }
    }



}


void planeApp::nextSegment(int direction){
    segment+=direction;
    segmentStart = ofGetUnixTime();
    if(segment >= scenes[scene].segments) {
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
			    Blob newb;
				blobs[blobid] = newb;
				blobs[blobid].id = blobid;
                blobs[blobid].perspectiveMat = &this->perspectiveMat;
//				cout << "NEW ";
			}

			// update blob with new values
			Blob* b = &blobs.find(blobid)->second;

            b->follow(posx, posy);
            b->setVelocity(velx, vely);
            b->analyze(freezeMinVel);
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
    int offsx = 10; int offsy = 10;

    this->drawRawData(offsx, offsy, 1.0);

    offsy += 190 + 10;
    this->drawTopDown(offsx, offsy, 0.5, drawBlobDetail);

    offsy = 10;
    offsx += 380;
    this->drawScreen(offsx, offsy, 0.35);

    offsx += 380;
    this->drawControlInfo(offsx, offsy);

    gui.draw();
}


//--------------------------------------------------------------
void planeApp::drawScreen(int x, int y, float scale){

    int projectionW = 1080;
    int projectionH = 1920;

    // frame first
    ofNoFill(); ofSetColor(255);
    ofRect(x,y,projectionW*scale,projectionH*scale);

    // display  instructions big
    string instruction = scenes[scene].instructions[segment];
    ofFill(); ofSetColor(255);
    font.drawString(instruction, x+20, y+projectionH*scale*0.1);

    if( scene==1 ){
        if( segment==0 ) {

            // STAND STILL
            // draw circle for each blob, and display frozen, frozentimer
            int bx = x + 100; int by = y + 250;
            ofFill();
            for(std::map<int, Blob>::iterator it = blobs.begin(); it != blobs.end(); ++it){
                Blob b = it->second;
                int add = b.frozenTimer < 10 ? b.frozenTimer*5 : 50;
                if(b.frozen) ofSetColor(200+add,0,200+add); else ofSetColor(100);
                ofCircle(bx, by, 65);

                string textStr = "id: " + ofToString(b.id);
                textStr += "\nvel:" + ofToString(b.vel, 2);
                textStr += "\nfrozen: "+ ofToString(b.frozen);
                textStr += "\nfrozenTimer: " + ofToString(b.frozenTimer);
                ofDrawBitmapStringHighlight(textStr, bx-60, by + 80);

                bx += 150;
                if(bx > x + projectionW*scale - 70) {
                    bx = x + 100; by += 240;
                }
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

    int blobserverW = 192;
    int blobserverH = 144;

    // frame first
    ofNoFill(); ofSetColor(255);
    ofRect(x,y,blobserverW*scale,blobserverH*scale);

    // write information
    string rawInfo = "port: " + ofToString(PORT) + "\nrate:";
    ofDrawBitmapStringHighlight(rawInfo, x + 3, y + blobserverH*scale + 15);

    // draw frame for each blob. blobserver frame size = 64 x 128 px
    for(std::map<int, Blob>::iterator it = blobs.begin(); it != blobs.end(); ++it){
        Blob b = it->second;
        ofRect( x + b._rawPos.x*scale, y + b._rawPos.y*scale, 64, 128);
    }

}

//--------------------------------------------------------------
void planeApp::drawTopDown(int x, int y, float scale, bool detailed) {

    int frameW = 640;
    int frameH = 480;

    // frame
    ofNoFill(); ofSetColor(255);
    ofRect(x,y,frameW*scale,frameH*scale);

    // write information
    string basicInfo = "blobs: " + ofToString(blobs.size());
    ofDrawBitmapStringHighlight(basicInfo, x + 3, y +frameH*scale+ 15);

    // draw history
    ofNoFill(); ofSetColor(150);
    for(std::map<int, Blob>::iterator it = blobs.begin(); it != blobs.end(); ++it){
        Blob b = it->second;
        ofBeginShape();
        for (vector<TimedPoint>::iterator it = b.history.begin() ; it != b.history.end(); ++it) {
            ofVertex( x + (*it).point.x*scale, y + (*it).point.y*scale );
        }
        ofEndShape();
    }

    // draw blobs as circles
    ofFill();
    for(std::map<int, Blob>::iterator it = blobs.begin(); it != blobs.end(); ++it){
        ofSetColor(255);
        Blob b = it->second;
        int bx = x + b.position.x*scale;
        int by = y + b.position.y*scale;
        ofCircle( bx, by, 20);

        // display blob details
        if(detailed) {
            string textStr = "id: " + ofToString(b.id);
            textStr += "\nvel:" + ofToString(b.velocity.x, 1) + "/" + ofToString(b.velocity.y, 1);
            textStr += "\nage: "+ofToString(b.age);
            textStr += "\nlost: " + ofToString(b.lostDuration);
            ofDrawBitmapStringHighlight(textStr, bx, by + 10);
        } else {
            ofFill(); ofSetColor(0);
            ofDrawBitmapString( ofToString(b.id, 4, '0'), bx-15, by+5);
        }

    }}


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
