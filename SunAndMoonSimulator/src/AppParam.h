//
//  AppParam.h
//

#pragma once
#include "ofMain.h"

namespace module{
    
    class AppParam{
        
        public:
        
        AppParam(){
            targetFps.addListener(this, &AppParam::targetFpsChanged);
        }
        
        ~AppParam(){
            targetFps.removeListener(this, &AppParam::targetFpsChanged);
        };
        
        void targetFpsChanged(int & fps){
            ofSetFrameRate(fps);
        }
        
        void set(){
            fps = ofGetFrameRate();

            // this makes app slower
            //bSmoothing ? ofEnableSmoothing() : ofDisableSmoothing();

            bAlphaBlending ? ofEnableAlphaBlending() : ofDisableAlphaBlending();
            bAntiAliasing ? ofEnableAntiAliasing() : ofDisableAntiAliasing();
            bDepthTest ? ofEnableDepthTest() : ofDisableDepthTest();
            ofSetVerticalSync(bVerticalSync);
            ofBackground(bg);
        }
        
        ofParameter<string> appVersion{"App Version", __DATE__};
        ofParameter<bool> bStart{"Start", false, false};
        ofParameter<bool> bDebug{"Debug", true};
        ofParameter<int> targetFps{"Target FPS", 50, 0, 500};
        ofParameter<int> fps{"FPS", -1, 0, 500, false};
        ofParameter<bool> bAntiAliasing{"Anti Aliasing", true};
        ofParameter<bool> bDepthTest{"Depth Test", false};
        ofParameter<bool> bAlphaBlending{"Alpha Blending", true};
        //ofParameter<bool> bSmoothing{"Smoothing", true};
        ofParameter<bool> bVerticalSync{"Vertical Sync", true};
        ofParameter<ofColor> bg{"Background Color", ofColor(0), ofColor(0), ofColor(255) };
        ofParameterGroup grp{"App", appVersion, bStart, bDebug, targetFps, fps, bAntiAliasing, bDepthTest, bAlphaBlending, /*bSmoothing,*/ bVerticalSync, bg};
        
    };
}

