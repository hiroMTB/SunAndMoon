#include "ofMain.h"
#include "ofApp.h"

int main( ){

    ofSetDataPathRoot("../Resources/data/");
    
    
	ofSetupOpenGL(1400,600,OF_WINDOW);
	ofRunApp(new ofApp());

}
