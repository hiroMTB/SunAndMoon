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
    ofSetIcoSphereResolution(4);

    ofSetFrameRate(60);
    ofSetVerticalSync(false);
    
    // gui
    gui.setup("settings", "json/settings.json");
    gui.add(appPrm.grp);
    gui.add(appGrp);
    gui.add(timeGrp);
    gui.add(geoGrp);
    gui.add(room.grp);
    gui.add(sunGrp);
    gui.add(moonGrp);
    gui.loadFromFile("json/settings.json");

    room.change();

    // set date NOW
    date = Poco::LocalDateTime();

    //cam.setFov(60);
    cam.setNearClip(100);
    cam.setFarClip(numeric_limits<float>::max()*0.001);

    update();
    
    int w = ofGetScreenWidth();
    timeline.allocate(w-20-100, 32);

    // 3D earth
    earthObj.loadModel("obj/earth.obj");
    earthObj.setRotation(0, 180, 1, 0, 0);
    earthObj.setRotation(1, 90, 0, 1, 0);
    earthObj.setScale(1.06, 1.06, 1.06);

    // ray
    vec3 sunRayDirection = glm::vec3(0,1,0);
    vec3 rayOrigin = glm::vec3(0, 0, 0);
    sunRay.setup(rayOrigin, sunRayDirection);
    moonRay.setup(rayOrigin, sunRayDirection);
    
}

void ofApp::update(){
    
    if(appPrm.bStart) {
        Poco::Timespan speed(0, 0, updateSpeed, 0, 0);
        date += speed;
    }
    
    sunpos = sun_calc.getSunPosition(date, lat, lon);
    todayInfo = sun_calc.getDayInfo(date, lat, lon, false);
    
    moonpos = sun_calc.getMoonPosition(date, lat, lon);

    ofxSunCalc::drawSimpleDayInfoTimeline(timeline, todayInfo);

    updateGui();
}

void ofApp::draw(){
    appPrm.set();
    ofSetLineWidth(1);
    
    bDrawSky ? drawSky()
              : ofBackground(0,0,50);
    
    draw3dDisplay();
    
    ofDisableDepthTest();
    drawBar();
    drawHeightDisplay();
    
    gui.draw();
}

void ofApp::draw3dDisplay(){

    static const float rad = 1000;
    static const float sunSize = 10;
    static const float moonSize = 5;

    static const vec3 up(0, 1, 0);
    static const vec3 north(0, 0, 1);
    static const vec3 east(-1, 0, 0);
    static const vec3 south(0, 0, -1);
    static const vec3 west( 1, 0, 0);

    {
        cam.begin();
        
        if(bDrawEarth){
            float earthRadius = 10000; //6378136.6 * 100;  // cm
            float floorY = room.height.get()/2;
            float oceanScale = 0.984;
            float landScale  = 0.98;
            
            ofPushMatrix();
            ofTranslate(0, -earthRadius-floorY);
            
            // Pin, Where we are
            if(!room.bDrawRoom){
                ofPushMatrix();
                ofSetColor(255, 0, 150);
                ofDrawLine(0,earthRadius+200, 0, earthRadius+400);
                ofFill();
                ofDrawSphere(vec3(0,earthRadius+400,0), 15);
                ofPopMatrix();
            }

            // ocean
            ofSetColor(0, 0, 50);
            ofFill();
            ofDrawIcoSphere(0, 0, earthRadius*oceanScale);

            ofRotateZDeg(90-lat);
            ofRotateYDeg(-lon);
            
            
            // City
            if(bDrawCity){
                for(auto & c : cityData.data){
                    ofPushMatrix();
                    ofRotateYDeg((180+c.lng));
                    ofRotateZDeg(90-c.lat);
                    ofTranslate(0, earthRadius+100);
                    ofFill();
                    ofSetColor(255);
                    ofDrawSphere(0, 0, 5);
                    //ofDrawBitmapString(c.name, 0, 0); // heavy
                    ofPopMatrix();
                }
            }
            
            // The earth
            ofPushMatrix();
            ofScale(earthRadius*landScale);
            ofScale(1.0/500);
            ofSetColor(120);
            earthObj.drawWireframe();
            ofPopMatrix();

            // Red line
            ofPushMatrix();
            ofRotateXDeg(90);
            ofSetColor(255, 0, 0 );
            ofNoFill();
            ofDrawCircle(0, 0, earthRadius*1.05);
            ofPopMatrix();
            
            // Pole
            ofSetColor(0, 0, 255);
            ofDrawLine(0, -earthRadius*1.2, 0, earthRadius*1.2);
            ofPopMatrix();
        }
        
        // important, look at south
        ofRotateYDeg(-90);
        
        if(bDrawSphere){
            ofDrawAxis(10);

            ofSetColor(255);
            ofPushMatrix();
            ofTranslate(0, -room.height/2);
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
            sunPosVec = sunPos2;
            
            if(bDrawSphere){
                ofSetColor(200);
                ofDrawLine(origin, sunPos1*rad+origin);
                ofDrawLine(sunPos2*rad+origin, sunPos3*rad+origin);
                
                ofSetColor(250, 0, 0);
                ofDrawLine(origin, sunPos2*rad+origin);

                ofFill();
                ofDrawSphere(sunPos2*rad + origin, sunSize);            
            }
            
            // ray
            sunRay.setDirection(glm::normalize(sunPos2));
            glm::vec3 baricentricCoordinates;
            glm::vec3 surfaceNormal;

            bool bHit = (sunRay.intersectsPrimitive(room.box, baricentricCoordinates, surfaceNormal));
            if(bHit){
                vec3 intersection = sunRay.getOrigin() + sunRay.getDirection() * baricentricCoordinates.z;
                room.addSunTrace(intersection);
            }

            // vbo
            sunPath.addVertex(sunPos2*rad + origin);

            ofSetColor(250, 0, 0);
            if(bDrawSphere){
                sunPath.drawVertices();
            
                // circle
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
            moonPosVec = moonPos2;
            
            if(bDrawSphere){
                ofSetColor(200);
                ofDrawLine(origin, moonPos1*rad+origin);
                ofDrawLine(moonPos2*rad+origin, moonPos3*rad+origin);
                
                ofSetColor(250, 250, 0);
                ofDrawLine(origin, moonPos2*rad+origin);

                ofFill();
                ofDrawSphere(moonPos2*rad+origin, moonSize);
            }
            
            // ray
            moonRay.setDirection(glm::normalize(moonPos2));
            glm::vec3 baricentricCoordinates;
            glm::vec3 surfaceNormal;
            
            bool bHit = (moonRay.intersectsPrimitive(room.box, baricentricCoordinates, surfaceNormal));
            if(bHit){
                vec3 intersection = moonRay.getOrigin() + moonRay.getDirection() * baricentricCoordinates.z;                
                room.addMoonTrace(intersection);
            }
            
            // vbo
            moonPath.addVertex(moonPos2*rad+origin);

            ofSetColor(250, 250, 0);

            if(bDrawSphere){
                moonPath.drawVertices();
            
                // circle
                ofSetColor(150);
                ofPushMatrix();
                ofTranslate(origin);
                ofRotateXDeg(90);
                ofNoFill();
                ofDrawCircle(0, 0, rad);
                ofPopMatrix();
            }
        }
        
        room.draw();

        cam.end();
    }
}

void ofApp::drawHeightDisplay(){
    
    // setup 2D viewport on the right
    float w = 60;
    float h = ofGetHeight()-80;
    float x = ofGetWidth()-w-15;
    float y = 10;
    ofRectangle v(x, y, w, h);
    ofPushView();
    ofViewport();
    ofSetupScreenOrtho();
    
    {
        // draw viewport edge and fill
        ofNoFill();
        ofSetLineWidth(1);
        ofSetColor(200);
        ofDrawRectangle(v);

        ofFill();
        ofSetColor(0);
        ofDrawRectangle(v);
    }
    
    {
        ofPushMatrix();
        ofTranslate(x, y);  // move to left top of 2D viewport
    
        float sunH = sunPosVec.get().y;   // normalized value
        float moonH = moonPosVec.get().y; // normalized value

        {
            // sun height line
            ofPushMatrix();
            ofTranslate(w/3, h/2);
            ofSetColor(255, 0, 0);
            ofDrawLine(0, +h/2, 0, -h/2);
            ofDrawCircle(0, -sunH*h/2, 3);
            ofPopMatrix();
        }
    
        {
            // moon height line
            ofPushMatrix();
            ofTranslate(2*w/3, h/2);
            ofSetColor(255, 250, 0);
            ofDrawLine(0, +h/2, 0, -h/2);
            ofDrawCircle(0, -moonH*h/2, 3);
            ofPopMatrix();
        }
    
        {
            // white zero line
            ofPushMatrix();
            ofTranslate(w/2, h/2);  // move to center
            ofSetColor(255);
            ofDrawLine(-w/2, 0, w/2, 0);
            ofPopMatrix();
        }
    
        ofPopMatrix();
    }
    
    ofPopView();
}

void ofApp::updateGui(){

    dateSt = Poco::DateTimeFormatter::format(date, "%Y-%m-%d");
    timeSt = Poco::DateTimeFormatter::format(date, "%H:%M:%S");

    sun_altitude = sunpos.altitude * RAD_TO_DEG;
    sun_azimuth  = sunpos.azimuth * RAD_TO_DEG;
    sun_dawn = sun_calc.dateToTimeString(todayInfo.dawn);
    sun_rise = sun_calc.dateToTimeString(todayInfo.sunrise.start);
    sun_noon = sun_calc.dateToTimeString(todayInfo.transit);
    sun_set = sun_calc.dateToTimeString(todayInfo.sunset.end);
    sun_dusk = sun_calc.dateToTimeString(todayInfo.dusk);
    
}

void ofApp::drawSky(){

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
    
    float tx = 20;
    float ty = ofGetHeight() - 50;

    ofSetColor(255);
    timeline.draw(tx, ty);
    
    // Draw a current time mark
    float pixels_per_min = (timeline.getWidth() / 24) / 60.0;
    float nx = tx + pixels_per_min * (date.hour() * 60 + date.minute());
    ofSetColor(255, 0, 0);
    ofSetLineWidth(1.0);
    ofDrawLine(nx, ty, nx, ty+timeline.getHeight());
    ty += timeline.getHeight() + 25;
}

void ofApp::clearVbo(){
    sunPath.clear();
    moonPath.clear();
    room.clear();
}

void ofApp::keyPressed(int key){

    switch(key){
            
        case ' ':
            appPrm.bStart = !appPrm.bStart;
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
            room.bDrawRoom = !room.bDrawRoom;
            break;

        case 's':
            bDrawSphere = !bDrawSphere;
            break;
            
        case 'e':
            bDrawEarth = !bDrawEarth;
            break;
            
        case 'b':
            bDrawSky = !bDrawSky;
            break;

        case 'C':
            bDrawCity = !bDrawCity;
            break;

    }
}
