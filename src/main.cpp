#include "ofMain.h"
#include "planeApp.h"
#include "ofAppGLFWWindow.h"

//========================================================================
int main( ){

	// ofSetupOpenGL(2520,1080,OF_FULLSCREEN);			// <-------- setup the GL context
	// ofSetupOpenGL(2520,1080,OF_WINDOW);			// <-------- setup the GL context
	// ofSetupOpenGL(1920,1080,OF_WINDOW);			// <-------- setup the GL context
	// ofSetupOpenGL(2520,880,OF_GAME_MODE);			// <-------- setup the GL context


	ofAppGLFWWindow window;  
    window.setMultiDisplayFullscreen(true);  
    // ofSetupOpenGL(&window,1440,900,OF_FULLSCREEN);
    ofSetupOpenGL(&window,1440,700,OF_WINDOW);

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp(new planeApp());

}
