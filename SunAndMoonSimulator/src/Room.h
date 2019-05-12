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
            
            roomVbo.setMode(OF_PRIMITIVE_LINES);
        }

        ~Room(){
            clear();
        }
    
        void clear(){
            moonWallPath.clear();
            sunWallPath.clear();    
        }

        void change(){

            // box
            box.set(width, height, depth, 1, 1, 1);
            box.setPosition(0, 0, 0);

            // construct room with lines since ofBox, ofPlane shows diagonal line
            {
                roomVbo.clear();

                int x = width/2;
                int y = height/2;
                int z = depth/2;

                {
                    // ceiling
                    roomVbo.addVertex(vec3( -x, y, z));
                    roomVbo.addVertex(vec3( x, y, z));
                    roomVbo.addVertex(vec3( x, y, z));
                    roomVbo.addVertex(vec3( x, y, -z));
                    roomVbo.addVertex(vec3( x, y, -z));
                    roomVbo.addVertex(vec3( -x, y, -z));
                    roomVbo.addVertex(vec3( -x, y, -z));
                    roomVbo.addVertex(vec3( -x, y, z));
                }
                
                {
                    //floor
                    roomVbo.addVertex(vec3( -x, -y, z));
                    roomVbo.addVertex(vec3( x,  -y, z));
                    roomVbo.addVertex(vec3( x,  -y, z));
                    roomVbo.addVertex(vec3( x,  -y, -z));
                    roomVbo.addVertex(vec3( x,  -y, -z));
                    roomVbo.addVertex(vec3( -x, -y, -z));
                    roomVbo.addVertex(vec3( -x, -y, -z));
                    roomVbo.addVertex(vec3( -x, -y, z));
                }
                
                {
                    // front wall
                    roomVbo.addVertex(vec3( -x, y, z));
                    roomVbo.addVertex(vec3( x, y, z));
                    roomVbo.addVertex(vec3( x, y, z));
                    roomVbo.addVertex(vec3( x, -y, z));
                    roomVbo.addVertex(vec3( x, -y, z));
                    roomVbo.addVertex(vec3( -x, -y, z));
                    roomVbo.addVertex(vec3( -x, -y, z));
                    roomVbo.addVertex(vec3( -x, y, z));
                }

                {
                    // back wall
                    roomVbo.addVertex(vec3( -x, y, -z));
                    roomVbo.addVertex(vec3( x, y, -z));
                    roomVbo.addVertex(vec3( x, y, -z));
                    roomVbo.addVertex(vec3( x, -y, -z));
                    roomVbo.addVertex(vec3( x, -y, -z));
                    roomVbo.addVertex(vec3( -x, -y, -z));
                    roomVbo.addVertex(vec3( -x, -y, -z));
                    roomVbo.addVertex(vec3( -x, y, -z));
                }
                
                {
                    // floor grid
                    int nGridW = width/20;
                    int nGridD = depth/20;
                    float gridW = width / nGridW;
                    float gridD = depth / nGridD;

                    // column line
                    for(int i=0; i<nGridW; i++){
                        int dx = i * gridW;
                        roomVbo.addVertex(vec3(-x+dx, -y, z));
                        roomVbo.addVertex(vec3(-x+dx, -y, -z));
                    }
                    
                    // row line
                    for(int i=0; i<nGridD; i++){
                        int dz = i * gridD;
                        roomVbo.addVertex(vec3(-x, -y,-z+dz));
                        roomVbo.addVertex(vec3(x, -y, -z+dz));
                    }
                }
            }
            
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
            
            ofPushMatrix();
            ofRotateXDeg(orientation.get().x);
            ofRotateYDeg(orientation.get().y);
            ofRotateZDeg(orientation.get().z);
            
            if(bDrawRoom){
                // room
                ofSetColor(150);
                roomVbo.draw();

                // sun
                ofSetColor(255, 0, 0);
                ofDrawSphere(sunOnTheWall, 3);
                sunWallPath.drawVertices();          

                // moon
                ofSetColor(255, 250, 0);
                ofDrawSphere(moonOnTheWall, 3);                
                moonWallPath.drawVertices();
            }
            
            // human
            ofSetColor(150);
            humanObj.drawWireframe();

            ofPopMatrix();
        }

        ofBoxPrimitive box;
        ofPlanePrimitive floor;
        
        ofVboMesh roomVbo;
        ofVboMesh sunWallPath;
        ofVboMesh moonWallPath;

        ofParameter<bool> bDrawRoom{"draw room", false};
        ofParameter<bool> bDrawCompass{"draw compass", false};
        ofParameter<float> width{"width (cm)", 750, 100, 1000};
        ofParameter<float> height{"height (cm)", 350, 100, 1000};
        ofParameter<float> depth{"depth (cm)", 900, 100, 1000};
        ofParameter<vec3>  orientation{"orientation", vec3(0), -vec3(180), vec3(180)};
        ofParameterGroup grp{"Room", bDrawRoom, bDrawCompass, width, height, depth, orientation};

        vec3 sunOnTheWall;
        vec3 moonOnTheWall;
        ofxAssimpModelLoader humanObj;

        ofEventListeners cbs;
    };
}
