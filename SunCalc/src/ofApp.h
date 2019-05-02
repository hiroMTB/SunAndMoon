#pragma once

#include "ofMain.h"
#include "ofxSunCalc.h"
#include "ofxRaycaster.h"

using namespace glm;

class ofApp : public ofBaseApp{
    
public:
    void setup();
    void update();

    void draw();
    
    void drawBar();
    void drawText();
    void drawBG();
    
    
    void keyPressed(int key);
    
    //void updateDebugStrings( Poco::LocalDateTime &date );
    
    ofxSunCalc sun_calc;
    SunCalcDayInfo todayInfo;
    SunCalcPosition sunpos;
    
    string min_info_str;
    string max_info_str;
    string pos_str;
    string date_str;
    string latlon_str;
    
    ofFbo timeline;
    string label;
    
    float lat;
    float lon;
    
    ofTrueTypeFont small_font;

    Poco::LocalDateTime date;

    float sun_brightness;
    
    
    ofEasyCam cam;

    glm::vec3 rayDirection;
    glm::vec3 rayOrigin;
    ofxraycaster::Ray ray;
    ofBoxPrimitive box;

    
    ofVboMesh sunPath;
    ofVboMesh sunWallPath;
    
    
    bool bStart = false;
    

};

