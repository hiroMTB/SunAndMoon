#pragma once

#include "ofMain.h"
#include "ofxAssimpModelLoader.h"
#include "ofxShadowMap.h"
#include "ofxGui.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
		
		ofxAssimpModelLoader model;
		ofxShadowMap shadowMap;
		ofEasyCam camera;
		ofLight light;

		ofParameter<float> fustrumSize{"fustrum size", 100, 10, 1000};
		ofParameter<float> farClip{"far clip", 300, 10, 1000};
		ofParameter<bool> enableShadows{"enable shaodws", true};
		ofParameterGroup parameters{
			"parameters",
			fustrumSize,
			farClip,
			enableShadows,
			shadowMap.parameters,
		};
		ofxPanel gui{parameters};

		ofPlanePrimitive ground{100,100,2,2};
		ofMaterial groundMaterial;
		ofEventListener listener;
    
    ofBoxPrimitive box;
};
