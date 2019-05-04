#pragma once

#include "ofMain.h"
#include "ofxSunCalc.h"
#include "ofxRaycaster.h"
#include "ofxAssimpModelLoader.h"

using namespace glm;

class ofApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    void draw3dDisplay();
    void drawHeightDisplay();
    void drawBar();
    void drawText();
    void drawBG();
    void changeDate(const Poco::Timespan & span);
    void clearVbo();
    void keyPressed(int key);
    
    ofxSunCalc sun_calc;
    SunCalcDayInfo todayInfo;
    SunCalcPosition sunpos;
    MoonCalcPosition moonpos;
    vec3 sunPosVec;
    vec3 moonPosVec;
    
    ofFbo timeline;
    
    bool bStart = false;
    bool bDrawRoom = false;
    bool bDrawSphere = true;
    bool bDrawEarth = true;

    float lat;
    float lon;
    float sun_brightness;

    ofTrueTypeFont small_font;
    Poco::LocalDateTime date;
    
    ofEasyCam cam;
    ofBoxPrimitive box;
    ofPlanePrimitive floor;
    
    ofxraycaster::Ray sunRay;
    ofxraycaster::Ray moonRay;
    ofVboMesh sunPath;
    ofVboMesh sunWallPath;
    ofVboMesh moonPath;
    ofVboMesh moonWallPath;
    
    ofxAssimpModelLoader model;
};
