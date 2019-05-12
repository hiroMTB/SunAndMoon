#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxAssimpModelLoader.h"
#include "ofxSunCalc.h"
#include "ofxRaycaster.h"
#include "Room.h"
#include "Earth.h"
#include "City.h"

#include "AppParam.h"

using namespace glm;
using namespace module;
using namespace sunandmoon;

class ofApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void updateGui();
    void draw();
    void draw3dDisplay();
    void drawHeightDisplay();
    void drawBar();
    void drawSky();
    void clearVbo();
    void keyPressed(int key);
    
    void exportTraj();

    // app param
    AppParam appPrm;
    
    // time
    ofParameter<string> dateSt{"date", "n.a."};
    ofParameter<string> timeSt{"time", "n.a."};
    ofParameter<int> updateSpeed{"update speed (min)", 1, 0, 60*24*30};
    ofParameterGroup timeGrp{"Time", dateSt, timeSt, updateSpeed};

    // sun
    ofParameter<bool > bDrawSun{"draw sun", true};
    ofParameter<float> sun_brightness{"sun brightness", 0, 0, 100};
    ofParameter<vec3> sunPosVec{"sun position (vec)", vec3(0), vec3(-1), vec3(1) };
    ofParameter<float> sun_altitude{"sun altitude", 0, -180, 180};
    ofParameter<float> sun_azimuth{"sun azimuth", 0, -180, 180};
    ofParameter<string> sun_dawn{"dawn", "n.a."};
    ofParameter<string> sun_rise{"sunrise", "n.a."};
    ofParameter<string> sun_noon{"solar noon", "n.a."};
    ofParameter<string> sun_set{"sunset", "n.a."};
    ofParameter<string> sun_dusk{"dusk", "n.a."};    
    ofParameterGroup sunGrp{"Sun", bDrawSun, sun_brightness, sunPosVec, sun_altitude, sun_azimuth, sun_dawn, sun_rise, sun_noon, sun_set, sun_dusk};

    // moon
    ofParameter<bool > bDrawMoon{"draw moon", true};
    ofParameter<float> moon_brightness{"moon brightness", 0, 0, 100};
    ofParameter<vec3> moonPosVec{"moon position (vec)", vec3(0), vec3(-1), vec3(1) };
    ofParameterGroup moonGrp{"Moon", bDrawMoon, moon_brightness, moonPosVec};

    // export
    ofParameter<bool> bExportSunSky{"export sun : sky trajectry", true};
    ofParameter<bool> bExportMoonSky{"export moon : sky trajectry", true};
    ofParameter<bool> bExportSunWall{"export sun : wall trajectry", true};
    ofParameter<bool> bExportMoonWall{"export moon : wall trajectry", true};
    ofParameter<void> btnExport{"Start Export"};
    ofParameterGroup expGrp{"Export", bExportSunSky, bExportMoonSky, bExportSunWall, bExportMoonWall, btnExport};
    
    ofxPanel gui;

    ofFbo timeline;        
    ofEasyCam cam;

    ofVboMesh sunPath;
    ofVboMesh moonPath;

    // ofx
    Poco::LocalDateTime date;
    ofxAssimpModelLoader earthObj;
    
    ofxraycaster::Ray sunRay;
    ofxraycaster::Ray moonRay;
    ofxSunCalc sun_calc;
    SunCalcDayInfo todayInfo;
    SunCalcPosition sunpos;
    MoonCalcPosition moonpos;

    Room room;

    Earth earth;

    ofEventListeners cbs;
};
