#pragma once

#include "ofMain.h"
#include "ofxAssimpModelLoader.h"

using namespace glm;

namespace sunandmoon{

    class Room{

        public:

        Room(){
                        
            // 3D humanObj
            humanObj.loadModel("obj/human.obj");
            humanObj.setScale(0.2, 0.2, 0.2);

            // listener
            cbs.push( width.newListener( [&](float & f){ change(); }));
            cbs.push( height.newListener([&](float & f){ change(); }));
            cbs.push( depth.newListener( [&](float & f){ change(); }));
            cbs.push( orientation.newListener( [&](vec3 & v){ change(); }));
        }

        ~Room(){}
    
        void clear(){
            moonWallPath.clear();
            sunWallPath.clear();    
        }

        void change(){

            // box
            box.set(width, height, depth, 1, 1, 1);
            box.setPosition(0, 0, 0);
            box.setUseVbo(true);

            // floor
            floor.set(width, depth, width/20, depth/20);
            floor.setPosition(0, -height/2, 0);
            floor.setOrientation(vec3(-90, 0, 0));
            floor.setUseVbo(true);

            // human
            humanObj.setPosition(0, -height/2, 0);
            humanObj.setRotation(0, 180, 1, 0, 0);
        }

        void addSunTrace(const vec3 & v){
            sunWallPath.addVertex(v);
            sunOnTheWall = v;
        }

        void addMoonTrace(const vec3 & v){
            moonWallPath.addVertex(v);
            moonOnTheWall = v;
        }

        void draw(){
            if(bDrawRoom){

                ofPushMatrix();
                ofRotateXDeg(orientation.get().x);
                ofRotateYDeg(orientation.get().y);
                ofRotateZDeg(orientation.get().z);

                // room
                ofSetColor(150);
                box.drawWireframe();
                
                // floor
                floor.drawWireframe();

                // person
                ofSetColor(150);
                humanObj.drawWireframe();

                // sun
                ofSetColor(255, 0, 0);
                ofDrawSphere(sunOnTheWall, 3);
                sunWallPath.drawVertices();          

                // moon
                ofSetColor(255, 250, 0);
                ofDrawSphere(moonOnTheWall, 3);                
                moonWallPath.drawVertices();

                ofPopMatrix();
            }
        }

        ofBoxPrimitive box;
        ofPlanePrimitive floor;

        ofVboMesh moonWallPath;
        ofVboMesh sunWallPath;

        ofParameter<bool> bDrawRoom{"draw room", false};
        ofParameter<float> width{"width (cm)", 750, 100, 1000};
        ofParameter<float> height{"height (cm)", 350, 100, 1000};
        ofParameter<float> depth{"depth (cm)", 900, 100, 1000};
        ofParameter<vec3>  orientation{"orientation", vec3(0), -vec3(180), vec3(180)};
        ofParameterGroup grp{"Room", bDrawRoom, width, height, depth, orientation};

        vec3 sunOnTheWall;
        vec3 moonOnTheWall;
        ofxAssimpModelLoader humanObj;

        ofEventListeners cbs;
    };
}
