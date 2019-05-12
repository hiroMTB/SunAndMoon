#include "ofApp.h"

#include "Poco/DateTimeFormatter.h"
#include "Poco/LocalDateTime.h"
#include "Poco/Timespan.h"

void ofApp::setup(){
    
    ofSetDataPathRoot("../Resources/data/");
    
    ofSetWindowTitle("SunCalc");
    ofSetWindowPosition(0, 0);
    ofSetWindowShape(1920, 1080);
    ofSetSphereResolution(12);
    ofSetCircleResolution(128);
    ofSetIcoSphereResolution(4);

    ofSetFrameRate(60);
    ofSetVerticalSync(false);
    
    // gui
    cbs.push(btnExport.newListener( [&](void){ exportTraj(); }));
    
    gui.setup("settings", "json/settings.json");
    gui.add(appPrm.grp);
    gui.add(timeGrp);
    gui.add(earth.grp);
    gui.add(sunGrp);
    gui.add(moonGrp);
    gui.add(room.grp);
    gui.add(expGrp);
    gui.loadFromFile("json/settings.json");
    room.change();
    
    gui.minimizeAll();

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
    
    float lat = earth.city.lat;
    float lng = earth.city.lng;

    sunpos = sun_calc.getSunPosition(date, lat, lng);
    todayInfo = sun_calc.getDayInfo(date, lat, lng, false);
    
    moonpos = sun_calc.getMoonPosition(date, lat, lng);

    ofxSunCalc::drawSimpleDayInfoTimeline(timeline, todayInfo);

    updateGui();
}

void ofApp::draw(){
    appPrm.set();
    ofSetLineWidth(1);
    
    earth.bDrawSky ? drawSky()
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
        earth.draw();
        
        vec3 origin = sunRay.getOrigin();
        
        ofDrawAxis(10);
        
        if(room.bDrawCompass){            
            ofPushMatrix();
            ofTranslate(origin);
            
            {
                ofPushMatrix();
                ofRotateXDeg(90);
                ofNoFill();
                ofSetColor(255);
                ofDrawCircle(0, 0, rad);
                ofDrawLine(vec2(-rad, 0), vec2(rad, 0));
                ofDrawLine(vec2(0, -rad), vec2(0, rad));
                ofPopMatrix();
            }
            
            ofTranslate(0, 20, 0);
            ofDrawBitmapString("N", north*rad);
            ofDrawBitmapString("E", east*rad);
            ofDrawBitmapString("S", south*rad);
            ofDrawBitmapString("W", west*rad);
        
            ofPopMatrix();
        }
        
        // calc sun position
        {
            float az = sunpos.azimuth - PI;     // 0 - 360, looks like something wrong in ofxSunCalc, so
            float alt = sunpos.altitude;        //
            
            vec3 sunPos1 = glm::rotate(north, az, -up);
            vec3 rotAxis = glm::rotate(east, az, -up);
            vec3 sunPos2 = glm::rotate(sunPos1, alt, rotAxis);  // this is the position
            vec3 sunPos3 = vec3(sunPos2.x, 0, sunPos2.z);       // this is projected position (z=0 plane)
            sunPosVec = sunPos2;
            
            // ray
            sunRay.setDirection(glm::normalize(sunPos2));

            int nMesh = room.roomObj.getNumMeshes();
            for(int i=0; i<nMesh; i++){
                glm::vec3 baricentricCoordinates;
                glm::vec3 surfaceNormal;

                const ofMesh & m = room.roomObj.getMesh(i);
                bool bHit = (sunRay.intersectsMesh(m, baricentricCoordinates, surfaceNormal));
                if(bHit){
                    vec3 intersection = sunRay.getOrigin() + sunRay.getDirection() * baricentricCoordinates.z;
                    room.addSunTrace(intersection);
                }
            }

            // vbo
            sunPath.addVertex(sunPos2*rad + origin);

            if(bDrawSun){

                // guide lines
                ofSetColor(200);
                ofDrawLine(origin, sunPos1*rad+origin);
                ofDrawLine(sunPos2*rad+origin, sunPos3*rad+origin);
                
                ofSetColor(250, 0, 0);
                ofDrawLine(origin, sunPos2*rad+origin);

                // sun on the sky
                ofFill();
                ofDrawSphere(sunPos2*rad + origin, sunSize);

                // sun traj on the sky
                ofSetColor(250, 0, 0);
                sunPath.drawVertices();
            }
        }
        
        // moon
        {
            float az = moonpos.azimuth - PI;     // 0 - 360, looks like something wrong in ofxSunCalc, so
            float alt = moonpos.altitude;        //
            
            vec3 moonPos1 = glm::rotate(north, az, -up);
            vec3 rotAxis = glm::rotate(east, az, -up);
            vec3 moonPos2 = glm::rotate(moonPos1, alt, rotAxis);  // this is the position
            vec3 moonPos3 = vec3(moonPos2.x, 0, moonPos2.z);       // this is projected position (z=0 plane)
            moonPosVec = moonPos2;
            
            
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

            if(bDrawMoon){
            
                // guide lines
                ofSetColor(200);
                ofDrawLine(origin, moonPos1*rad+origin);
                ofDrawLine(moonPos2*rad+origin, moonPos3*rad+origin);
                
                ofSetColor(250, 250, 0);
                ofDrawLine(origin, moonPos2*rad+origin);
                
                // moon on the sky
                ofFill();
                ofDrawSphere(moonPos2*rad+origin, moonSize);
                
                // traj on the sky
                ofSetColor(250, 250, 0);
                moonPath.drawVertices();
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
            if(ofGetKeyPressed(OF_KEY_SHIFT)){
                earth.city.goToNextCity();
            }else{
                date += Poco::Timespan(1,0,0,0,0);
            }
            break;
    
        case OF_KEY_LEFT:
            if(ofGetKeyPressed(OF_KEY_SHIFT)){
                earth.city.goToPrevCity();
            }else{
                date -= Poco::Timespan(1,0,0,0,0);
            }
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
            
        case 'e':
            earth.bDrawEarth = !earth.bDrawEarth;
            break;
            
        case 'C':
            earth.city.bDrawCity = !earth.city.bDrawCity;
            break;

    }
}

void ofApp::exportTraj(){
    
    string message = "Export .ply file, please select file prefix name";
    string defaultFilename = "sm_";
    ofFileDialogResult saveFileResult = ofSystemSaveDialog(defaultFilename, message);
    if (saveFileResult.bSuccess){
        
        string filename = saveFileResult.filePath;
        
        if(bExportSunSky){
            sunPath.save(filename + "_sun-sky.ply");
        }
        
        if(bExportMoonSky){
            moonPath.save(filename + "_moon-sky.ply");
        }
        
        if(bExportSunWall){
            room.sunWallPath.save(filename + "_sun-wall.ply");
        }
        
        if(bExportMoonWall){
            room.moonWallPath.save(filename + "_moon-wall.ply");
        }
        
    }else{
        cout << "error" << endl;
    }
}
