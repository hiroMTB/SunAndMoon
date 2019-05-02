#include "ofApp.h"

#include "Poco/DateTimeFormatter.h"
#include "Poco/LocalDateTime.h"
#include "Poco/Timespan.h"

void ofApp::setup(){
    
    ofSetWindowTitle("SunCalc");
    ofSetWindowPosition(0, 0);
    ofSetWindowShape(1920, 1080);
    
    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    
    // set date NOW
    date = Poco::LocalDateTime();

    // Berlin, Germany
    lat = 52.52;
    lon = 13.40;
    
    latlon_str = "lat:" + ofToString(lat) + ", lon:" + ofToString(lon);
    
    
    update();
    
    ofLogNotice("date") << date_str;
    //ofLogNotice("local tzd") << date.tzd();
    
    ofLogNotice("today") << sun_calc.dateToString(date);
    ofLogNotice("sunpos") << pos_str;
    ofLogNotice() << min_info_str << endl << endl << max_info_str;
    
    small_font.load(OF_TTF_MONO, 8, false);
    
    label = "Today\n" +  ofxSunCalc::dateToDateString(date);
        
    // create/draw a timeline for each date
    timeline.allocate(ofGetWidth() - 20 - 110, 32);
    ofxSunCalc::drawSimpleDayInfoTimeline(timeline, todayInfo);
    
    
    ofSetSphereResolution(12);
    //ofSetIcoSphereResolution(4);
    ofSetCircleResolution(128);
    

    // ray
    rayDirection = glm::vec3(0,1,0);
    rayOrigin = glm::vec3(0, 0, 0);
    ray.setup(rayOrigin, rayDirection);
    
    // box
    box.set(200, 200, 300, 1, 1, 1);
    box.setPosition(0, 0, 0);

    glm::quat q;
    
    box.setOrientation(vec3(0, 30, 0));
 

}

void ofApp::update(){

    
    if(bStart) {
        // auto step the time of day to proof changes
        // int total_min = fabs(sin(ofGetElapsedTimef()*.05)) * 1440;
        int total_min = (int)((ofGetElapsedTimef()*.05) * 1440) % 1440;// 1440 == mins per day  60 * 24
        int hr = floor(total_min/60.0);
        int mn = total_min - (hr*60); //date.minute();
        date.assign(date.year(), date.month(), date.day(), hr, mn);
    }
    
    
    date_str = Poco::DateTimeFormatter::format(date, "%Y-%m-%d %H:%M:%S");
    sunpos = sun_calc.getSunPosition(date, lat, lon);
    pos_str = "altitude=" + ofToString(sunpos.altitude * RAD_TO_DEG, 2) + ", azimuth=" + ofToString(sunpos.azimuth * RAD_TO_DEG,2);
    todayInfo = sun_calc.getDayInfo(date, lat, lon, true);
    min_info_str = sun_calc.infoToString(todayInfo, true);
    max_info_str = sun_calc.infoToString(todayInfo, false);    
}

void ofApp::draw(){

    ofEnableDepthTest();
    ofEnableAntiAliasing();
    ofEnableAlphaBlending();

    //drawBG();
    ofBackground(255);
    
    float rad = 400;
    
    
    vec3 up(0, 1, 0);
    vec3 north(0, 0, 1);
    vec3 east(-1, 0, 0);
    vec3 south(0, 0, -1);
    vec3 west( 1, 0, 0);
    
    
    ofSetLineWidth(1);
    {
        cam.begin();

        ofDrawAxis(30);

        ofNoFill();
        ofSetColor(30, 250);
        ofDrawBitmapString("N", north*rad);
        ofDrawBitmapString("E", east*rad);
        ofDrawBitmapString("S", south*rad);
        ofDrawBitmapString("W", west*rad);
        
        
        // calc sun position
        float az = sunpos.azimuth - PI;     // 0 - 360, looks like something wrong in ofxSunCalc, so
        float alt = sunpos.altitude;        //
        
        vec3 sunPos1 = glm::rotate(north, az, -up);
        vec3 rotAxis = glm::rotate(east, az, -up);
        vec3 sunPos2 = glm::rotate(sunPos1, alt, rotAxis);  // this is the position
        vec3 sunPos3 = vec3(sunPos2.x, 0, sunPos2.z);       // this is projected position (z=0 plane)
        
        ofSetColor(200);
        ofDrawLine(vec3(0), sunPos1*rad);
        ofDrawLine(sunPos2*rad, sunPos3*rad);
        
        ofSetColor(250, 50, 50);
        ofDrawLine(vec3(0), sunPos2*rad);
        
        
        // ray
        ray.setDirection(glm::normalize(sunPos2));
        glm::vec3 baricentricCoordinates;
        glm::vec3 surfaceNormal;

        bool bHit = (ray.intersectsPrimitive(box, baricentricCoordinates, surfaceNormal));
        if(bHit){
            auto intersection = ray.getOrigin() + ray.getDirection() * baricentricCoordinates.z;

            ofSetColor(255, 0, 0);
            ofDrawSphere(intersection, 1);

            // reflected light
            auto reflLight = glm::reflect(ray.getDirection(),surfaceNormal);
            ofSetColor(50, 100);
            ofDrawLine(intersection, intersection + 20 * reflLight);
            
            // vbo
            sunWallPath.addVertex(intersection);

        }


        // vbo
        sunPath.addVertex(sunPos2*rad);

        ofSetColor(50, 250);
        sunPath.drawVertices();
        sunWallPath.drawVertices();
        
        
        
        // sun
        ofFill();
        ofSetColor(255, 0, 0);
        ofDrawSphere(sunPos2*rad, 1);
        
        
        // room
        ofSetColor(100, 50);
        box.drawWireframe();
        
        // circle
        ofSetColor(150);
        ofPushMatrix();
        ofRotateXDeg(90);
        ofNoFill();
        ofDrawCircle(0, 0, rad);
        ofPopMatrix();
        cam.end();
    }
    
    ofDisableDepthTest();
    drawText();
    drawBar();
    
}

void ofApp::drawText(){
    
    ofDrawBitmapStringHighlight(date_str, 15, 20, ofColor::paleGoldenRod, ofColor::black);
    ofDrawBitmapStringHighlight(min_info_str, 15, 45, ofColor::salmon, ofColor::white);
    ofDrawBitmapStringHighlight(max_info_str, 15, 125, ofColor::darkorange, ofColor::white);
    ofDrawBitmapStringHighlight(latlon_str, 195, 20, ofColor::gold, ofColor::black);
    ofDrawBitmapStringHighlight(pos_str, 195, 45, ofColor::cornsilk, ofColor::black);
    ofDrawBitmapStringHighlight("Current Brightness " + ofToString(sun_brightness, 3), 195, 70, ofColor::goldenRod, ofColor::white);
}

void ofApp::drawBG(){

    sun_brightness = ofxSunCalc::getSunBrightness(todayInfo, date);
    
    if(ofGetKeyPressed(OF_KEY_COMMAND)) {
        sun_brightness = fabs(sin(ofGetElapsedTimef()*.1));
    }
    
    // draw background gradient based on sun_brightness
    
    ofColor nightBG(ofColor::black);
    ofColor nightFG(64);
    
    ofColor dayBG(ofColor::skyBlue);
    ofColor dayFG(ofColor::paleGoldenRod);
    
    ofColor background = nightBG.lerp(dayBG, sun_brightness);
    ofColor foreground = nightFG.lerp(dayFG, sun_brightness);
    
    
    ofBackgroundGradient(foreground, background);
    
}

void ofApp::drawBar(){
    
    float tx = 10 + 110;
    float ty = ofGetHeight() - 50;;

    ofSetColor(255);
    timeline.draw(tx, ty);
    ofDrawBitmapStringHighlight(label, 10, ty+13);
    ofNoFill();
    ofSetLineWidth(1.0);
    ofSetColor(255);
    ofDrawRectangle(tx, ty, timeline.getWidth(), timeline.getHeight());
    
    // Draw a current time mark
    float pixels_per_min = (timeline.getWidth() / 24) / 60.0;
    float nx = tx + pixels_per_min * (date.hour() * 60 + date.minute());
    ofSetColor(255, 0, 0);
    ofSetLineWidth(2.0);
    ofDrawLine(nx, ty, nx, ty+timeline.getHeight());
    ty += timeline.getHeight() + 25;
}

void ofApp::keyPressed(int key){

    switch(key){
            
        case ' ':
            bStart = !bStart;
            break;
            
        case OF_KEY_RIGHT:
            date += Poco::Timespan(1,0,0,0,0);
            break;

    
        case OF_KEY_LEFT:
            date -= Poco::Timespan(1,0,0,0,0);
            break;

            
        case OF_KEY_UP:
            date += Poco::Timespan(30,0,0,0,0);
            break;

            
        case OF_KEY_DOWN:
            date -= Poco::Timespan(30,0,0,0,0);
            break;

    }
}

