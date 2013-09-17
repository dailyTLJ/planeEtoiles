#include "planeApp.h"

//--------------------------------------------------------------
void planeApp::setup(){
	// listen on the given port
	cout << "listening for osc messages on port " << PORT << "\n";
	receiver.setup(PORT);

	current_msg_string = 0;
	mouseX = 0;
	mouseY = 0;
	mouseButtonState = "";

	ofBackground(30, 30, 130);

}

//--------------------------------------------------------------
void planeApp::update(){


	// check for waiting messages
	while(receiver.hasWaitingMessages()){

		// get the next message
		ofxOscMessage m;
		receiver.getNextMessage(&m);

		// check for startFrame
		if(m.getAddress() == "/blobserver/startFrame"){
			cout << "\n";
		}
		// check for endFrame
		else if(m.getAddress() == "/blobserver/endFrame"){

		}
		// check for bgsubtractor
		else if(m.getAddress() == "/blobserver/bgsubtractor"){

		}
		// check for stitch
		else if(m.getAddress() == "/blobserver/stitch"){

		}
		// check for stitch
		else if(m.getAddress() == "/blobserver/nop"){

		}
		// check for hog
		else if(m.getAddress() == "/blobserver/hog"){
			// parse incoming elements:  iiffiii: x y vx vy id age lost
			int blobid = m.getArgAsInt32(4);
			int posx = m.getArgAsInt32(0);
			int posy = m.getArgAsInt32(1);
			float velx = m.getArgAsFloat(2);
			float vely = m.getArgAsFloat(3);
			int age = m.getArgAsInt32(5);
			int lost = m.getArgAsInt32(6);

			std::map<int,Blob>::iterator iter = blobs.find(blobid);
			if( iter != blobs.end() ) {		// blob instance already exists

			} else {					// create new blob instance
			    Blob newb;
				blobs[blobid] = newb;
				blobs[blobid].id = blobid;
//				cout << "NEW ";
			}

			// update blob with new values
			Blob b = blobs.find(blobid)->second;

            b.follow(posx, posy);
            b.velocity.set(velx, vely);
            b.age = age;
            b.lostDuration = lost;

			cout << "blob " << blobid << "   " << posx << "|" << posy << "   - lifetime: " << b.lifetime << "\n";
		}
		else{
			// unrecognized message
		}

	}

	// check for dead blobs, and delete them from list
    std::map<int,Blob>::iterator it = blobs.begin();
    while (it != blobs.end()) {
    	it->second.update();
    	if( !it->second.isAlive() ) {
//            cout << "delete blob " << it->second.id << "\n";
    		blobs.erase(it++);
    	} else {
    		++it;
    	}
    }

//    cout << " . ";

}

//--------------------------------------------------------------
void planeApp::draw(){

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
