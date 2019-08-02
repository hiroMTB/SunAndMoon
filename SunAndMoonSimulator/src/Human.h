//
//  Human.h
//

#pragma once

#include "ofxAssimpModelLoader.h"
#include "ofxManipulator.h"

using glm::vec3;

namespace sunandmoon{
    
    class Human{
        
    public:
        
        Human(){
            // 3D humanObj
            humanObj.loadModel("obj/human.obj");
            
            float scale = height * 0.001;
            humanObj.setScale(scale, scale, scale);
            humanObj.setPosition(0, 0, 0);
            humanObj.setRotation(0, 180, 1, 0, 0);
            
            gizmo.setScale(ofVec3f(scale, scale, scale));
            gizmo.setTranslation(ofVec3f(0,0,0));
            gizmo.setManipulatorType(ofxManipulator::MANIPULATOR_NONE);
            gizmo.setManipulatorScale(0.8);
            gizmo.setOnFocusThreshold(80);
            gizmo.setOutFocusThreshold(160);
            
            ofAddListener(gizmo.onTranslateChange, this, &Human::onTranslateChange);
            ofAddListener(gizmo.onFocus, this, &Human::onFocus);
            ofAddListener(gizmo.outFocus, this, &Human::outFocus);
            
            cbs.push( pos.newListener( [&](vec3 & v){ changePositionFromGui(); }));
        }
        
        void draw(ofBoxPrimitive & box, const ofCamera & cam){
            
            // human
            ofPushMatrix();
            ofTranslate(box.getPosition());
            ofTranslate(0, -box.getHeight()/2, 0);
            
            vec3 orientatino = box.getOrientationEulerDeg();
            ofRotateXDeg(orientatino.x);
            ofRotateZDeg(orientatino.z);
            ofRotateYDeg(orientatino.y);
            ofSetColor(150);
            humanObj.drawWireframe();
            
            if(1){
                // gizmo always need to be drawn in order to update position.
                ofDisableLighting();
                ofFill();
                gizmo.draw(cam);
            }
            
            ofPopMatrix();
        }

        void onTranslateChange(ofVec3f & p){
            humanObj.setPosition(p.x, p.y, p.z);
            pos = p;
        }

        void onScaleChange(ofVec3f & s){
            humanObj.setScale(s.x, s.y, s.z);
        }

        void onRotateChange(ofVec3f & r){
            
        }
        
        void onFocus(ofVec3f & cur){
            gizmo.setManipulatorType(ofxManipulator::MANIPULATOR_TRANSLATION);
            ofSendMessage("lockCamera");
        }

        void outFocus(ofVec3f & cur){
            gizmo.setManipulatorType(ofxManipulator::MANIPULATOR_NONE);
            ofSendMessage("unlockCamera");
        }
        
        void changePositionFromGui(){
            const vec3 & p = pos.get();
            humanObj.setPosition(p.x, p.y, p.z);
            gizmo.setTranslation(ofVec3f(p.x, p.y, p.z));
        }
        
        ofParameter<bool> bDraw{"draw", false};
        ofParameter<vec3> pos{"position", vec3(0), vec3(-1000), vec3(1000)};
        ofParameter<float> height{"height", 165, 100, 185};
        ofParameterGroup grp{"Human", bDraw, pos, height};
        
        ofxAssimpModelLoader humanObj;
        ofxManipulator gizmo;
        
        ofEventListeners cbs;
    };
}
