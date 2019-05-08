#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxAssimpModelLoader.h"
#include "ofxSunCalc.h"
#include "ofxRaycaster.h"
#include "Room.h"
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

    // app param
    AppParam appPrm;
    
    // app
    ofParameter<bool> bDrawSky{"draw sky", false};
    ofParameter<bool> bDrawCity{"draw city", false};
    ofParameter<bool> bDrawSphere{"draw sphere", false};
    ofParameter<bool> bDrawEarth{"draw earth", true};
    ofParameterGroup appGrp{"app", bDrawSky, bDrawCity, bDrawSphere, bDrawEarth};

    // geo (default Berlin)
    ofParameter<float> lat{"latitude", 52.52, -60, 60};
    ofParameter<float> lon{"longtitude", 13.40, -180, 180};
    ofParameterGroup geoGrp{"geo", lat, lon};

    // time
    ofParameter<string> dateSt{"date", "n.a."};
    ofParameter<string> timeSt{"time", "n.a."};
    ofParameter<int> updateSpeed{"update speed (min)", 1, 0, 60*24*30};
    ofParameterGroup timeGrp{"time", dateSt, timeSt, updateSpeed};

    // sun
    ofParameter<float> sun_brightness{"sun brightness", 0, 0, 100};
    ofParameter<vec3> sunPosVec{"sun position (vec)", vec3(0), vec3(-1), vec3(1) };
    ofParameter<float> sun_altitude{"sun altitude", 0, -180, 180};
    ofParameter<float> sun_azimuth{"sun azimuth", 0, -180, 180};

    ofParameter<string> sun_dawn{"dawn", "n.a."};
    ofParameter<string> sun_rise{"sunrise", "n.a."};
    ofParameter<string> sun_noon{"solar noon", "n.a."};
    ofParameter<string> sun_set{"sunset", "n.a."};
    ofParameter<string> sun_dusk{"dusk", "n.a."};    
    ofParameterGroup sunGrp{"sun", sun_brightness, sun_dawn, sun_rise, sun_noon, sun_set, sun_dusk};

    // moon
    ofParameter<float> moon_brightness{"moon brightness", 0, 0, 100};
    ofParameter<vec3> moonPosVec{"moon position (vec)", vec3(0), vec3(-1), vec3(1) };
    ofParameterGroup moonGrp{"moon", moon_brightness};

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

    CityData cityData;
};
