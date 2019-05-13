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
#include "AppParam.h"

using namespace glm;
using namespace module;
using namespace sunandmoon;

class ofApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void updateTime();
    void calcWallTrj();
    void draw();
    void draw3dDisplay();
    void drawHeightDisplay();
    void drawBar();
    void drawSky();
    void clearVbo();
    void keyPressed(int key);

    // app param
    AppParam appPrm;
    
    // time
    ofParameter<string> utcTimeSt{"UTC time", "n.a."};
    ofParameter<string> localTimeSt{"Local time", "n.a."};
    ofParameter<int> updateSpeed{"update speed (min)", 1, 0, 60*24*30};
    ofParameterGroup timeGrp{"Time", utcTimeSt, localTimeSt, updateSpeed};

    // Reset
    ofParameter<void> btnClearTrj{"Clear trajectory"};
    ofParameter<void> btnResetTime{"Reset time to NOW"};
    ofParameterGroup rstGrp{"Reset", btnClearTrj, btnResetTime};
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
    
    #include "Exporter.h"
};
