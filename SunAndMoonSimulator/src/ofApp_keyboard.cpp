#include "ofApp.h"

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
                syncLocalToUtc();
                updateTimeString();
            }
            break;
            
        case OF_KEY_LEFT:
            if(ofGetKeyPressed(OF_KEY_SHIFT)){
                earth.city.goToPrevCity();
            }else{
                utcDate -= Poco::Timespan(1,0,0,0,0);
                syncLocalToUtc();
                updateTimeString();
            }
            break;
            
        case OF_KEY_UP:
            utcDate += Poco::Timespan(30,0,0,0,0);
            syncLocalToUtc();
            updateTimeString();
            break;
            
        case OF_KEY_DOWN:
            utcDate -= Poco::Timespan(30,0,0,0,0);
            syncLocalToUtc();
            updateTimeString();
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
            
        case 'l':
            cam.getMouseInputEnabled() ? cam.disableMouseInput() : cam.enableMouseInput();
            break;
    }
}
