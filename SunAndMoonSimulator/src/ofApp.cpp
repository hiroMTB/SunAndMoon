#include "ofApp.h"

#include "Poco/DateTimeFormatter.h"
#include "Poco/LocalDateTime.h"
#include "Poco/Timespan.h"
#include "Constants.h"

void ofApp::setup(){
    
    ofSetWindowTitle("SunAndMoonSimulator" );
    
    int sw = ofGetScreenWidth();
    int sh = ofGetScreenHeight();
    
    ofSetWindowPosition(0, 0);
    ofSetWindowShape(sw, sh);
    ofSetSphereResolution(12);
    ofSetCircleResolution(128);
    ofSetIcoSphereResolution(4);
    
    // gui
    cbs.push(btnExport.newListener( [&](void){ exportTrj(); }));
    cbs.push(trjDrawMode.newListener([&](int & mode){changeTrjDrawMode(mode);}));
    
    // reset
    cbs.push(btnClearTrj.newListener( [&](void){clearVbo(); }));
    cbs.push(btnResetTime.newListener( [&](void){ utcDate = Poco::LocalDateTime().utc(); }));
    
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
    gui.add(trjGrp);
    gui.add(expGrp);
    gui.loadFromFile(settingPath);
    gui.minimizeAll();
    
    room.change();
    window.plane.setParent(room.box);
    window.reset();
    
    //cam.setFov(60);
    cam.setNearClip(100);
    cam.setFarClip(numeric_limits<float>::max()*0.001);
}

void ofApp::update(){
    
    updateTime();
    
    float lat = earth.city.lat;
    float lng = earth.city.lng;
    int tz = earth.city.timezone = round(lng / 180.0 * 12.0);
    sun.update(utcDate, lat, lng, tz);
    moon.update(utcDate, lat, lng, tz);
    
    calcIntersection();
    calcIntersectionWindowRay(window.moonRayVbo, window.moonRays, -moon.ray.getDirection(), moon.pos.y, ofColor(250, 250, 0, 100));
    calcIntersectionWindowRay(window.sunRayVbo, window.sunRays, -sun.ray.getDirection(), sun.pos.y, ofColor(250, 0, 0, 10, 0));
}

void ofApp::updateTime(){
    
    int tz = earth.city.timezone;
    Poco::Timespan offset(0,tz,0,0,0);
    
    Poco::Timespan speed(0, 0, updateSpeed, 0, 0);
    if(appPrm.bStart) {
        utcDate += speed;
    }
    localDate = utcDate + offset;
    
    // update string display
    utcTimeSt = Poco::DateTimeFormatter::format(utcDate, "%H:%M:%S");
    localTimeSt = Poco::DateTimeFormatter::format(localDate, "%H:%M:%S");
    
    utcDateSt = Poco::DateTimeFormatter::format(utcDate, "%Y-%m-%d");
    localDateSt = Poco::DateTimeFormatter::format(localDate, "%Y-%m-%d");
    
}

void ofApp::calcIntersection(){
    
    {
        // Sun and Room
        glm::vec3 bari;
        glm::vec3 normal;
        ofxraycaster::Ray & ray = sun.ray;
        bool bHit = (ray.intersectsPrimitive(room.box, bari, normal));
        if(bHit){
            vec3 intersection = ray.getOrigin() + ray.getDirection() * bari.z;
            room.addSunTrace(intersection);
        }
    }
    
    {
        // Moon and Room
        ofxraycaster::Ray & ray = moon.ray;
        glm::vec3 bari;
        glm::vec3 normal;
        bool bHit = (ray.intersectsPrimitive(room.box, bari, normal));
        if(bHit){
            vec3 intersection = ray.getOrigin() + ray.getDirection() * bari.z;
            room.addMoonTrace(intersection);
        }
    }
}

void ofApp::calcIntersectionWindowRay( ofVboMesh & vbo, vector<Ray> & rays, vec3 dir, float planetHeight,
                                      ofColor col){
    
    float refLen = 100;
    
    int nV = vbo.getVertices().size();
    int nRay = window.nRow * window.nCol;
    if(nV !=  nRay*4){
        ofLogError() << "Something is wrong here";
        ofLogError() << "vbo size = " << nV;
        ofLogError() << "nCol = " << window.nCol;
        ofLogError() << "nRow = " << window.nRow;
        ofLogError() << "nRay = " << nRay;
        return;
    }
    
    bool bPlanetIsHigh = (planetHeight > 0);
    if(!bPlanetIsHigh){
        for(int r=0; r<window.nRow; r++){
            for(int c=0; c<window.nCol; c++){
                int index = c + window.nCol * r;
                int vboIndex = index * 4;
                for(int i=0; i<4; i++){
                    vbo.setVertex(vboIndex+i, vec3(0));
                    vbo.setColor(vboIndex+i, ofColor(0,0,0,0));
                }
            }
        }
    }else{
        ofMesh & mesh = window.plane.getMesh();
        
        for(int r=0; r<window.nRow; r++){
            for(int c=0; c<window.nCol; c++){
                int index = c + window.nCol * r;
                const vec3 & v = mesh.getVertex(index);
                Ray & ray = window.moonRays[index];
                vec3 origin = window.pos.get() + v + dir;
                ray.setOrigin(origin);
                ray.setDirection(dir);
                glm::vec3 bari;
                glm::vec3 normal;
                bool bHit = (ray.intersectsPrimitive(room.box, bari, normal));
                int vboIndex = index * 4;
                if(bHit){
                    vec3 intersection = origin + dir * bari.z;
                    vbo.setVertex(vboIndex+0, origin);
                    vbo.setVertex(vboIndex+1, intersection);
                    
                    //reflection
                    vec3 refDir = glm::reflect(dir, normal);
                    vbo.setVertex(vboIndex+2, intersection);
                    vbo.setVertex(vboIndex+3, intersection + refDir * refLen);
      
                    vbo.setColor(vboIndex+0, col);
                    vbo.setColor(vboIndex+1, col);
                    vbo.setColor(vboIndex+2, col);
                    vbo.setColor(vboIndex+3, col);
                }else{
                    vbo.setVertex(vboIndex+0, origin);
                    vbo.setVertex(vboIndex+1, origin + dir * 10);
                    vbo.setVertex(vboIndex+2, origin);  // no reflection, ignore
                    vbo.setVertex(vboIndex+3, origin);  // no reflection, ignore
                    
                    for(int i=0; i<4; i++){
                        vbo.setColor(vboIndex+i, ofColor(0));
                    }
                }
            }
        }
    }
}

void ofApp::draw(){
    appPrm.set();
    earth.bDrawSky ? drawSky() : ofBackground(0,0,50);
    
    // 3d
    draw3dDisplay();
    
    // 2d
    ofDisableDepthTest();
    drawBar();
    drawHeightDisplay();
    gui.draw();
}

void ofApp::draw3dDisplay(){
    
    const float rad = 1000;
    
    {
        cam.begin();
        earth.draw();
        
        vec3 origin = sun.ray.getOrigin();
        
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
        
        sun.draw();
        moon.draw();
        room.draw();
        window.draw();
        
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

void ofApp::keyPressed(int key){
    
    switch(key){
            
        case ' ':
            appPrm.bStart = !appPrm.bStart;
            break;
            
        case OF_KEY_RIGHT:
            if(ofGetKeyPressed(OF_KEY_SHIFT)){
                earth.city.goToNextCity();
            }else{
                utcDate += Poco::Timespan(1,0,0,0,0);
            }
            break;
            
        case OF_KEY_LEFT:
            if(ofGetKeyPressed(OF_KEY_SHIFT)){
                earth.city.goToPrevCity();
            }else{
                utcDate -= Poco::Timespan(1,0,0,0,0);
            }
            break;
            
        case OF_KEY_UP:
            utcDate += Poco::Timespan(30,0,0,0,0);
            break;
            
        case OF_KEY_DOWN:
            utcDate -= Poco::Timespan(30,0,0,0,0);
            break;
            
        case 'c':
            clearVbo();
            break;
            
        case 'r':
            room.bDraw = !room.bDraw;
            break;
            
        case 'e':
            earth.bDrawEarth = !earth.bDrawEarth;
            break;
            
        case 'C':
            earth.city.bDrawCity = !earth.city.bDrawCity;
            break;
            
    }
}
