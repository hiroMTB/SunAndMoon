#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxAssimpModelLoader.h"
#include "ofxRaycaster.h"
#include "Sun.h"
#include "Moon.h"
#include "Earth.h"
#include "City.h"
#include "Room.h"
#include "Window.h"
#include "AppParam.h"

using namespace glm;
using namespace module;
using namespace sunandmoon;

class ofApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void updateTime();
    void calcIntersection();
    void calcIntersectionWindowRay( vector<Ray> & rays, vec3 dir, float planetHeight, ofColor col);
    void draw();
    void draw3dDisplay();
    void drawHeightDisplay();
    void drawBar();
    void drawSky();
    void clearVbo();
    void keyPressed(int key);
    void changeTrjDrawMode(int & mode);
    
    // app param
    AppParam appPrm;
    
    // time
    ofParameter<string> utcDateSt{"UTC date", "n.a."};
    ofParameter<string> utcTimeSt{"UTC time", "n.a."};
    ofParameter<string> localDateSt{"Local date", "n.a."};
    ofParameter<string> localTimeSt{"Local time", "n.a."};    
    ofParameter<int> updateSpeed{"update speed (min)", 1, 0, 60*24*30};
    ofParameter<void> btnResetTime{"Reset time to NOW"};
    ofParameterGroup timeGrp{"Time", utcDateSt, utcTimeSt, localDateSt, localTimeSt, updateSpeed, btnResetTime};

    // Trajectory
    ofParameter<void> btnClearTrj{"Clear trajectory"};
    ofParameter<int> trjDrawMode{"draw mode", 1, 0, 3};
    ofParameterGroup trjGrp{"Trajectory", trjDrawMode, btnClearTrj};
    
    ofxPanel gui;
    ofEasyCam cam;
    ofxAssimpModelLoader earthObj;

    Poco::DateTime utcDate;
    Poco::DateTime localDate;

    Room room;
    Earth earth;
    Sun sun;
    Moon moon;

    ofEventListeners cbs;

    Window window;
    
    #include "Exporter.h"
};
