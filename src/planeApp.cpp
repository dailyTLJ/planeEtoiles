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

	// hide old messages
	for(int i = 0; i < NUM_MSG_STRINGS; i++){
		if(timers[i] < ofGetElapsedTimef()){
			msg_strings[i] = "";
		}
	}

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
			// ignore
			int posx = m.getArgAsInt32(0);
			int posy = m.getArgAsInt32(1);
			float velx = m.getArgAsFloat(2);
			float vely = m.getArgAsFloat(3);
			int blobid = m.getArgAsInt32(4);
			int age = m.getArgAsInt32(5);
			int lost = m.getArgAsInt32(6);

			Blob b = new Blob();
			b.id = blobid;

			blobs[blobid] = b;

			cout << "blob " << blobid << "   " << posx << "|" << posy << "\n";
		}
		else{
			// unrecognized message: display on the bottom of the screen
			string msg_string;
			msg_string = m.getAddress();
			msg_string += ": ";
			for(int i = 0; i < m.getNumArgs(); i++){
				// get the argument type
				msg_string += m.getArgTypeName(i);
				msg_string += ":";
				// display the argument - make sure we get the right type
				if(m.getArgType(i) == OFXOSC_TYPE_INT32){
					msg_string += ofToString(m.getArgAsInt32(i));
				}
				else if(m.getArgType(i) == OFXOSC_TYPE_FLOAT){
					msg_string += ofToString(m.getArgAsFloat(i));
				}
				else if(m.getArgType(i) == OFXOSC_TYPE_STRING){
					msg_string += m.getArgAsString(i);
				}
				else{
					msg_string += "unknown";
				}
			}
			// add to the list of strings to display
			msg_strings[current_msg_string] = msg_string;
			timers[current_msg_string] = ofGetElapsedTimef() + 5.0f;
			current_msg_string = (current_msg_string + 1) % NUM_MSG_STRINGS;
			// clear the next line
			msg_strings[current_msg_string] = "";
		}

	}

	// check for dead blobs, and delete them from list
	// for (std::map<int,Blob>::iterator it=blobs.begin(); it!=blobs.end(); ++it)
 //    	it->id

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
