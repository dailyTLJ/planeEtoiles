#include "planeApp.h"
#include "ofBitmapFont.h"

//--------------------------------------------------------------
void planeApp::setup(){
	// listen on the given port
	cout << "listening for osc messages on port " << PORT << "\n";
	receiver.setup(PORT);

	mouseX = 0;
	mouseY = 0;
	mouseButtonState = "";

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
    stars.instructions[0] = "Try new spots to \nlight up  more stars.";
    stars.length[0] = 20;
    stars.instructions[0] = "Walk with someone. \nKeep the same distance \nbetween you.";
    stars.length[0] = 30;
    stars.instructions[0] = "Walk with someone. \nMake eye contact. \nKeep the distance.";
    stars.length[0] = 30;
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
    if(segmentClock > 10){
        segment++;
        segmentStart = ofGetUnixTime();
        if(segment >= scenes[scene].segments) {
            scene++;
            segment = 0;
            if(scene >= scenes.size()) {
                scene = 0;
            }
        }
    }

    // analysis
    if (scene==0) {
        if (segment==0) {

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
            b->velocity.set(velx, vely);
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

    // draw raw data / small display
    //--------------------------------------------------------------
    ofNoFill(); ofSetColor(255); ofRect(offsx,offsy,192,144);
    string rawInfo = "port: " + ofToString(PORT) + "\nrate:";
    ofDrawBitmapStringHighlight(rawInfo, offsx + 192 + 4, offsy + 13);
    for(std::map<int, Blob>::iterator it = blobs.begin(); it != blobs.end(); ++it){
        Blob b = it->second;
        ofRect( offsx + b._rawPos.x, offsy + b._rawPos.y, 64, 128);
    }

    // draw top down view
    //--------------------------------------------------------------
    float md = 0.5; // multiplay display size
    offsy += 190 + 10;
    ofNoFill(); ofSetColor(255); ofRect(offsx,offsy,640*md,480*md);
    string basicInfo = "blobs: " + ofToString(blobs.size());
    ofDrawBitmapStringHighlight(basicInfo, offsx + 640*md + 4, offsy + 13);

    // draw history
    ofNoFill(); ofSetColor(150);
    for(std::map<int, Blob>::iterator it = blobs.begin(); it != blobs.end(); ++it){
        Blob b = it->second;
        ofBeginShape();
        for (vector<TimedPoint>::iterator it = b.history.begin() ; it != b.history.end(); ++it) {
            ofVertex( offsx + (*it).point.x*md, offsy + (*it).point.y*md );
        }
        ofEndShape();
    }
    // draw blobs
    ofFill();
    for(std::map<int, Blob>::iterator it = blobs.begin(); it != blobs.end(); ++it){
        ofSetColor(255);
        Blob b = it->second;
        ofCircle( offsx + b.position.x*md, offsy + b.position.y*md, 20);

        int x = offsx + b.position.x*md;
        int y = offsy + b.position.y*md;
        string textStr = "id: " + ofToString(b.id);
        textStr += "\nvel:" + ofToString(b.velocity.x, 1) + "/" + ofToString(b.velocity.y, 1);
        textStr += "\nage: "+ofToString(b.age);
        textStr += "\nlost: " + ofToString(b.lostDuration);
        ofDrawBitmapStringHighlight(textStr, x, y + 10);
    }


    // draw scene control
    offsy = 10;
    ofNoFill(); ofSetColor(255); ofRect(offsx+400,offsy,360,640);

    ofFill(); ofSetColor(255);


    ofDrawBitmapString("SCENE ID\t" + ofToString(scene) +
                       "\nSCENE NAME\t" + scenes[scene].name +
                       "\nSEGMENT\t\t" + ofToString(segment) +
                       "\n\n" + scenes[scene].instructions[segment] +
                       "\n\nGLOBAL TIME\t" + ofToString(masterClock) +
                       "\nSEGMENT TIME\t" + ofToString(segmentClock), offsx+400+360+3,offsy + 10 );

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

}

//--------------------------------------------------------------
void planeApp::keyPressed(int key){

}

//--------------------------------------------------------------
void planeApp::keyReleased(int key){

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
