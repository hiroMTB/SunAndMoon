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
    
    string min_info_str;
    string max_info_str;
    string pos_str;
    string date_str;
    string latlon_str;
    
    ofFbo timeline;
    string label;
    
    bool bStart = false;
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
    
    bool bRoomView = false;

    ofxAssimpModelLoader model;
};
