#include "ofApp.h"

#include "Poco/DateTimeFormatter.h"
#include "Poco/DateTimeParser.h"
#include "Poco/LocalDateTime.h"
#include "Poco/Timespan.h"
#include "Constants.h"

const string ofApp::pocoDateFormat{"%Y-%m-%d"};
const string ofApp::pocoTimeFormat{"%H:%M:%S"};

void ofApp::setup(){
    
    ofSetWindowTitle("SunAndMoonSimulator" );
    
    int sw = ofGetScreenWidth();
    int sh = ofGetScreenHeight();
    
    ofSetWindowPosition(0, 0);
    ofSetWindowShape(sw, sh);
    ofSetSphereResolution(12);
    ofSetCircleResolution(128);
    ofSetIcoSphereResolution(4);
    
    setupGui();
    
    earth.setup();
    room.setup();
    window.setup(room.box);
    
    //cam.setFov(60);
    cam.setNearClip(100);
    cam.setFarClip(numeric_limits<float>::max()*0.001);
    
    //
    shadowMap.setup(4096*2);
    shadowMap.setupMaterialWithShadowMap(groundMaterial);
    ground.setOrientation(glm::angleAxis(ofDegToRad(-90.f), glm::vec3{1.f,0.f,0.f}));
    box.set(100, 100, 100);
    box.setPosition(0, 50, 0);
    
    light.enable();
    shadowMapListener = enableShadows.newListener([this](bool & on){
        if(!on){
            shadowMap.begin(light, fustrumSize, 1, farClip);
            shadowMap.end();
        }
    });
}

void ofApp::setupGui(){
    
    addPrmListener();
    
    string settingPath = ofToDataPath("json/settings.json");
    gui.setup("settings", settingPath);
    gui.add(appPrm.grp);
    gui.add(timeGrp);
    gui.add(earth.grp);
    gui.add(earth.city.grp);
    gui.add(sun.grp);
    gui.add(moon.grp);
    gui.add(room.grp);
    gui.add(window.grp);
    gui.add(human.grp);
    gui.add(trjGrp);
    gui.add(shadowGrp);
    gui.add(shadowMapGrp);
    gui.add(expGrp);
    gui.loadFromFile(settingPath);
    gui.minimizeAll();
}

void ofApp::addPrmListener(){
    cbs.push(btnExport.newListener( [&](void){ exportTrj(); }));
    cbs.push(trjDrawMode.newListener([&](int & mode){ changeTrjDrawMode(mode);}));
    cbs.push(btnClearTrj.newListener( [&](void){ clearVbo(); }));
    cbs.push(btnResetTime.newListener( [&](void){ utcDate = Poco::LocalDateTime().utc(); syncLocalToUtc(); updateTimeString();}));
    cbs.push(utcDateSt.newListener( [&](string&){ setDateTimeByUtcString(); }));
    cbs.push(utcTimeSt.newListener( [&](string&){ setDateTimeByUtcString(); }));
    cbs.push(localDateSt.newListener( [&](string&){ setDateTimeByLocalString(); }));
    cbs.push(localTimeSt.newListener( [&](string&){ setDateTimeByLocalString(); }));
}

void ofApp::removePrmListener(){
    cbs.unsubscribeAll();
}

void ofApp::setDateTimeByUtcString(){
    
    const string fmt = pocoDateFormat + " " + pocoTimeFormat;
    const string utcSt = utcDateSt.get() + "" + utcTimeSt.get();
    int tz = 0;
    bool ok = Poco::DateTimeParser::tryParse(fmt, utcSt, utcDate, tz);
    if(ok){
        syncLocalToUtc();
        updateTimeString();
    }
}

void ofApp::setDateTimeByLocalString(){
    
    const string fmt = pocoDateFormat + " " + pocoTimeFormat;
    const string localSt = localDateSt.get() + "" + localTimeSt.get();
    int tz = 0;
    bool ok = Poco::DateTimeParser::tryParse(fmt, localSt, localDate, tz);
    if(ok){
        syncUtcToLocal();
        updateTimeString();
    }
}

void ofApp::update(){
    
    float lat = earth.city.lat;
    float lng = earth.city.lng;
    int tz = earth.city.timezone = round(lng / 180.0 * 12.0);
    
    updateTime();
    
    sun.update(utcDate, lat, lng, tz);
    moon.update(utcDate, lat, lng, tz);
    
    // update ray origin based on human's head position
    vec3 origin = human.pos + vec3(0,165,0);
    sun.ray.setOrigin(origin);
    moon.ray.setOrigin(origin);
    
    calcIntersectionWall();
    calcIntersectionWindow(window.moonRays, -moon.ray.getDirection(), moon.pos.y, ofColor(250, 250, 0, 100));
    calcIntersectionWindow(window.sunRays, -sun.ray.getDirection(), sun.pos.y, ofColor(250, 0, 0, 100));
    
    calcShadow();
}

void ofApp::updateTime(){
    
    // update utc
    if(appPrm.bStart) {
        Poco::Timespan speed(0, 0, updateSpeed, 0, 0);
        utcDate += speed;
        syncLocalToUtc();
        updateTimeString();
    }
}

void ofApp::syncLocalToUtc(){
    int tz = earth.city.timezone;
    Poco::Timespan offset(0,tz,0,0,0);
    localDate = utcDate + offset;
}

void ofApp::syncUtcToLocal(){
    int tz = earth.city.timezone;
    Poco::Timespan offset(0,tz,0,0,0);
    utcDate = localDate - offset;
}

void ofApp::updateTimeString(){
    
    removePrmListener();
    
    // update string display
    utcDateSt = Poco::DateTimeFormatter::format(utcDate, pocoDateFormat);
    utcTimeSt = Poco::DateTimeFormatter::format(utcDate, pocoTimeFormat);
    
    localDateSt = Poco::DateTimeFormatter::format(localDate, pocoDateFormat);
    localTimeSt = Poco::DateTimeFormatter::format(localDate, pocoTimeFormat);
    
    addPrmListener();
}

void ofApp::calcShadow(){
    
    if(sun.altitude>0){
        shadowDirection = fmodf(sun.azimuth + 180.0f, 360.0);
        shadowLength = stickLength * tan( ofDegToRad(90.0 - sun.altitude));
    }else{
        shadowDirection = 0;
        shadowLength = 0;
    }
}

void ofApp::draw(){
    appPrm.set();
    earth.bDrawSky ? drawSky() : ofBackground(0,0,50);
    
    // 3d
    ofEnableLighting();
    draw3dDisplay();
    
    // 2d
    ofDisableDepthTest();
    drawBar();
    drawHeightDisplay();
    
    gui.draw();

//    shadowMap debug
//    ofSetColor(255);
//    shadowMap.getDepthTexture().draw(ofGetWidth()-256-10, 0, 256, 256);
}

void ofApp::draw3dDisplay(){
    
    {
        ofEnableDepthTest();
        ofEnableLighting();
        float longitude = -sun.azimuth;
        float latitude = -sun.altitude;
        float dist = sun.radius;
        
        light.orbitDeg(longitude, latitude, dist, {0,0,0});
        
        ground.setPosition( vec3(0) );
        shadowMap.begin(light, fustrumSize, 1, farClip);
        ofPushMatrix();
        room.draw();
        human.draw(room.box, cam);
        ofPopMatrix();
        shadowMap.end();
        
        shadowMap.updateMaterial(groundMaterial);
    }
    
    const float rad = 1000;
    
    {
        cam.begin();
        earth.draw();
        
        ofDrawAxis(10);
        
        if(room.bDrawCompass){
            ofDisableLighting();
            ofPushMatrix();
            
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
            ofEnableLighting();
        }
        
        sun.draw();
        moon.draw();
        room.draw();
        window.drawWindow();
        human.draw(room.box, cam);
        
//        groundMaterial.begin();
//        ground.draw();
//        groundMaterial.end();
        //light.draw();
        
        if(bDrawTrj){
            if(sun.bDraw) window.drawSunRays();
            if(moon.bDraw) window.drawMoonRays();
        }
        
        // shadow
        ofPushMatrix();
        //ofTranslate(origin);
        ofRotateYDeg(-shadowDirection);
        ofSetColor(0,255,0);
        ofDrawLine(vec3(0), vec3(0, 0, shadowLength));
        ofPopMatrix();
        
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
        
        float sunH = sun.pos.y;   // normalized value
        float moonH = moon.pos.y;           // normalized value
        
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

void ofApp::drawSky(){
    
    float brightness = sun.brightness;
    ofColor nightBG(ofColor::black);
    ofColor nightFG(64);
    ofColor dayBG(ofColor::skyBlue);
    ofColor dayFG(ofColor::paleGoldenRod);
    ofColor background = nightBG.lerp(dayBG, brightness);
    ofColor foreground = nightFG.lerp(dayFG, brightness);
    ofBackgroundGradient(foreground, background);
}

void ofApp::drawBar(){
    
    float tx = 20;
    float ty = ofGetHeight() - 50;
    
    ofSetColor(255);
    sun.timeline.draw(tx, ty);
    
    // Draw a current time mark
    int tz = earth.city.timezone;
    float pixels_per_min = (sun.timeline.getWidth() / 24) / 60.0;
    float nx = tx + pixels_per_min * ((utcDate.hour()+tz) * 60 + utcDate.minute());
    ofSetColor(255, 0, 0);
    ofSetLineWidth(1.0);
    ofDrawLine(nx, ty, nx, ty+sun.timeline.getHeight());
    ty += sun.timeline.getHeight() + 25;
}

void ofApp::clearVbo(){
    sun.clear();
    moon.clear();
    room.clear();
}

void ofApp::changeTrjDrawMode(int & i){
    
    ofPrimitiveMode mode;
    switch(i){
        case 0:
            mode = OF_PRIMITIVE_POINTS;
            break;
        case 1:
            mode = OF_PRIMITIVE_LINE_STRIP;
            break;
        case 2:
            mode = OF_PRIMITIVE_LINES;
            break;
        case 3:
            mode = OF_PRIMITIVE_LINE_LOOP;
            break;
    }
    
    sun.trj.setMode(mode);
    moon.trj.setMode(mode);
    room.sunWallPath.setMode(mode);
    room.moonWallPath.setMode(mode);
}

void ofApp::messageReceived(ofMessage & message){

    if(message.message == "lockCamera"){
        cam.disableMouseInput();
    }
    else if(message.message == "unlockCamera"){
        cam.enableMouseInput();
    }    
}

