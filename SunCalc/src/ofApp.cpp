#include "ofApp.h"

#include "Poco/DateTimeFormatter.h"
#include "Poco/LocalDateTime.h"
#include "Poco/Timespan.h"

void ofApp::setup(){
    
    ofSetWindowTitle("SunCalc");
    ofSetWindowPosition(0, 0);
    ofSetWindowShape(1920, 1080);
    ofSetSphereResolution(12);
    ofSetCircleResolution(128);
    
    ofSetFrameRate(60);
    ofSetVerticalSync(false);
    
    // set date NOW
    date = Poco::LocalDateTime();

    // Berlin, Germany
    lat = 52.52;
    lon = 13.40;
    
    latlon_str = "lat:" + ofToString(lat) + ", lon:" + ofToString(lon);
    
    update();
    
    small_font.load(OF_TTF_MONO, 8, false);
    
    label = ofxSunCalc::dateToDateString(date);
        
    // create/draw a timeline for each date
    timeline.allocate(ofGetWidth() - 20 - 110, 32);
    ofxSunCalc::drawSimpleDayInfoTimeline(timeline, todayInfo);
    

    // box
    float orY = 30;
    float width = 700;
    float height = 350;
    float depth = 900;
    box.set(width, height, depth, 1, 1, 1);
    box.setPosition(0, 0, 0);
    box.setOrientation(vec3(0, orY, 0));
    box.setUseVbo(true);

    // floor
    floor.set(width, depth, width/20, depth/20);
    floor.setPosition(0, -height/2, 0);
    floor.setOrientation(vec3(-90, 0, orY));
    floor.setUseVbo(true);
    
    // 3D model
    model.loadModel("Human_806polys.obj");
    model.setScale(0.2, 0.2, 0.2);
    model.setRotation(0, 180, 1, 0, 0);
    model.setRotation(1, orY, 0, 1, 0);
    model.setPosition(0, -height/2, 0);

    // ray
    vec3 sunRayDirection = glm::vec3(0,1,0);
    vec3 rayOrigin = glm::vec3(0, -height/2+165, 0);
    sunRay.setup(rayOrigin, sunRayDirection);
    moonRay.setup(rayOrigin, sunRayDirection);
    
}

void ofApp::update(){
    
    if(bStart) {
        Poco::Timespan speed(0, 0, 2, 0, 0);
        date += speed;
    }
    
    date_str = Poco::DateTimeFormatter::format(date, "%Y-%m-%d %H:%M:%S");
    sunpos = sun_calc.getSunPosition(date, lat, lon);
    pos_str = "altitude=" + ofToString(sunpos.altitude * RAD_TO_DEG, 2) + ", azimuth=" + ofToString(sunpos.azimuth * RAD_TO_DEG,2);
    todayInfo = sun_calc.getDayInfo(date, lat, lon, false);
    min_info_str = sun_calc.infoToString(todayInfo, true);
    max_info_str = sun_calc.infoToString(todayInfo, false);
    
    moonpos = sun_calc.getMoonPosition(date, lat, lon);
}

void ofApp::draw(){

    ofEnableDepthTest();
    ofEnableAntiAliasing();
    ofEnableAlphaBlending();
    ofSetLineWidth(1);
    
    //drawBG();
    ofBackground(0,0,40);
    
    float rad = 800;
    
    vec3 up(0, 1, 0);
    vec3 north(0, 0, 1);
    vec3 east(-1, 0, 0);
    vec3 south(0, 0, -1);
    vec3 west( 1, 0, 0);


    {
        cam.begin();
        
        if(!bRoomView){
            ofDrawAxis(10);

            ofSetColor(255);
            ofPushMatrix();
            ofTranslate(0, -box.getHeight()/2);
            ofDrawBitmapString("N", north*rad);
            ofDrawBitmapString("E", east*rad);
            ofDrawBitmapString("S", south*rad);
            ofDrawBitmapString("W", west*rad);
            ofPopMatrix();
        }
        
        // calc sun position
        {
            vec3 origin = sunRay.getOrigin();
            float az = sunpos.azimuth - PI;     // 0 - 360, looks like something wrong in ofxSunCalc, so
            float alt = sunpos.altitude;        //
            
            vec3 sunPos1 = glm::rotate(north, az, -up);
            vec3 rotAxis = glm::rotate(east, az, -up);
            vec3 sunPos2 = glm::rotate(sunPos1, alt, rotAxis);  // this is the position
            vec3 sunPos3 = vec3(sunPos2.x, 0, sunPos2.z);       // this is projected position (z=0 plane)
            
            if(!bRoomView){
                ofSetColor(200);
                ofDrawLine(origin, sunPos1*rad+origin);
                ofDrawLine(sunPos2*rad+origin, sunPos3*rad+origin);
                
                ofSetColor(250, 0, 0);
                ofDrawLine(origin, sunPos2*rad+origin);
            }
            
            // ray
            sunRay.setDirection(glm::normalize(sunPos2));
            glm::vec3 baricentricCoordinates;
            glm::vec3 surfaceNormal;

            bool bHit = (sunRay.intersectsPrimitive(box, baricentricCoordinates, surfaceNormal));
            if(bHit){
                auto intersection = sunRay.getOrigin() + sunRay.getDirection() * baricentricCoordinates.z;

                ofSetColor(255, 0, 0);
                ofDrawSphere(intersection, 3);

                // reflected light
                auto reflLight = glm::reflect(sunRay.getDirection(),surfaceNormal);
                ofSetColor(50, 100);
                ofDrawLine(intersection, intersection + 20 * reflLight);
                
                // vbo
                sunWallPath.addVertex(intersection);
            }

            // vbo
            sunPath.addVertex(sunPos2*rad + origin);

            ofSetColor(250, 0, 0);
            if(!bRoomView){
                sunPath.drawVertices();
            }
            sunWallPath.drawVertices();
            
            // sun
            if(!bRoomView){
                ofFill();
                ofDrawSphere(sunPos2*rad + origin, 3);
            }
            
            // circle
            if(!bRoomView){
                ofSetColor(150);
                ofPushMatrix();
                ofTranslate(origin);
                ofRotateXDeg(90);
                ofNoFill();
                ofDrawCircle(0, 0, rad);
                ofPopMatrix();
            }
        }
        
        // moon
        {
            vec3 origin = moonRay.getOrigin();
            
            float az = moonpos.azimuth - PI;     // 0 - 360, looks like something wrong in ofxSunCalc, so
            float alt = moonpos.altitude;        //
            
            vec3 moonPos1 = glm::rotate(north, az, -up);
            vec3 rotAxis = glm::rotate(east, az, -up);
            vec3 moonPos2 = glm::rotate(moonPos1, alt, rotAxis);  // this is the position
            vec3 moonPos3 = vec3(moonPos2.x, 0, moonPos2.z);       // this is projected position (z=0 plane)
            
            if(!bRoomView){
                ofSetColor(200);
                ofDrawLine(origin, moonPos1*rad+origin);
                ofDrawLine(moonPos2*rad+origin, moonPos3*rad+origin);
                
                ofSetColor(250, 250, 0);
                ofDrawLine(origin, moonPos2*rad+origin);
            }
            
            // ray
            moonRay.setDirection(glm::normalize(moonPos2));
            glm::vec3 baricentricCoordinates;
            glm::vec3 surfaceNormal;
            
            bool bHit = (moonRay.intersectsPrimitive(box, baricentricCoordinates, surfaceNormal));
            if(bHit){
                auto intersection = moonRay.getOrigin() + moonRay.getDirection() * baricentricCoordinates.z;
                
                ofSetColor(250, 250, 0);
                ofDrawSphere(intersection, 3);
                
                // reflected light
                auto reflLight = glm::reflect(moonRay.getDirection(),surfaceNormal);
                ofSetColor(50, 100);
                ofDrawLine(intersection, intersection + 20 * reflLight);
                
                // vbo
                moonWallPath.addVertex(intersection);
            }
            
            // vbo
            moonPath.addVertex(moonPos2*rad+origin);

            ofSetColor(250, 250, 0);

            if(!bRoomView){
                moonPath.drawVertices();
            }
            
            moonWallPath.drawVertices();
            
            // moon
            if(!bRoomView){
                ofFill();
                ofDrawSphere(moonPos2*rad+origin, 3);
            }
            
            // circle
            if(!bRoomView){
                ofSetColor(150);
                ofPushMatrix();
                ofTranslate(origin);
                ofRotateXDeg(90);
                ofNoFill();
                ofDrawCircle(0, 0, rad);
                ofPopMatrix();
            }
        }
        
        // room
        ofSetColor(150);
        box.drawWireframe();
        
        // floor
        floor.drawWireframe();

        // person
        model.drawWireframe();
    
        cam.end();
    }
    
    ofDisableDepthTest();
    drawText();
    drawBar();
    
    ofDrawBitmapStringHighlight("fps : " + ofToString(ofGetFrameRate()), 50, 50);
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

void ofApp::changeDate(const Poco::Timespan & span){
    date += span;
}

void ofApp::clearVbo(){
    sunPath.clear();
    sunWallPath.clear();
    moonPath.clear();
    moonWallPath.clear();
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

        case 'c':
            clearVbo();
            break;

        case 'r':
            bRoomView = !bRoomView;
            break;

    }
}
