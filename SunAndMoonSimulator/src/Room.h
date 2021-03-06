#pragma once

#include "ofMain.h"

using namespace glm;

namespace sunandmoon{

    class Room{

        public:

        Room(){}
        
        void setup(){

            // listener
            cbs.push( width.newListener( [&](float & f){ change(); }));
            cbs.push( height.newListener([&](float & f){ change(); }));
            cbs.push( depth.newListener( [&](float & f){ change(); }));
            cbs.push( orientation.newListener( [&](vec3 & v){ change(); }));
            
            roomVbo.setMode(OF_PRIMITIVE_LINES);
            
            change();
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
            float offsetY = height/2.0;
            box.set(width, height, depth, 1, 1, 1);
            box.setPosition(0, offsetY, 0);
            box.setOrientation(orientation);
            
            // construct room with lines since ofBox, ofPlane shows diagonal line
            {
                roomVbo.clear();

                int x = width/2;
                int y = height;
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
                    roomVbo.addVertex(vec3( -x, 0, z));
                    roomVbo.addVertex(vec3( x,  0, z));
                    roomVbo.addVertex(vec3( x,  0, z));
                    roomVbo.addVertex(vec3( x,  0, -z));
                    roomVbo.addVertex(vec3( x,  0, -z));
                    roomVbo.addVertex(vec3( -x, 0, -z));
                    roomVbo.addVertex(vec3( -x, 0, -z));
                    roomVbo.addVertex(vec3( -x, 0, z));
                }
                
                {
                    // front wall
                    roomVbo.addVertex(vec3( -x, y, z));
                    roomVbo.addVertex(vec3( x, y, z));
                    roomVbo.addVertex(vec3( x, y, z));
                    roomVbo.addVertex(vec3( x, 0, z));
                    roomVbo.addVertex(vec3( x, 0, z));
                    roomVbo.addVertex(vec3( -x, 0, z));
                    roomVbo.addVertex(vec3( -x, 0, z));
                    roomVbo.addVertex(vec3( -x, y, z));
                }

                {
                    // back wall
                    roomVbo.addVertex(vec3( -x, y, -z));
                    roomVbo.addVertex(vec3( x, y, -z));
                    roomVbo.addVertex(vec3( x, y, -z));
                    roomVbo.addVertex(vec3( x, 0, -z));
                    roomVbo.addVertex(vec3( x, 0, -z));
                    roomVbo.addVertex(vec3( -x, 0, -z));
                    roomVbo.addVertex(vec3( -x, 0, -z));
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
                        roomVbo.addVertex(vec3(-x+dx, 0, z));
                        roomVbo.addVertex(vec3(-x+dx, 0, -z));
                    }
                    
                    // row line
                    for(int i=0; i<nGridD; i++){
                        int dz = i * gridD;
                        roomVbo.addVertex(vec3(-x, 0,-z+dz));
                        roomVbo.addVertex(vec3(x, 0, -z+dz));
                    }
                }
            }
        }

        void addSunTrace(const vec3 & v){
            vec3 vLocal = ceonvertToRoomCoordinate(v);
            sunWallPath.addVertex( vLocal );
            sunOnTheWall = vLocal;
        }

        void addMoonTrace(const vec3 & v){
            vec3 vLocal = ceonvertToRoomCoordinate(v);
            moonWallPath.addVertex( vLocal );
            moonOnTheWall = vLocal;
        }

        vec3 ceonvertToRoomCoordinate(const vec3 & v){
            mat4 m = box.getGlobalTransformMatrix();
            //mat4 m = box.getLocalTransformMatrix();
            mat4 inv = glm::inverse(m);
            vec4 v4 = vec4(v, 1);
            vec4 r4 = inv * v4;
            float offsetY = box.getPosition().y;
            return vec3(r4.x, r4.y + offsetY, r4.z);
        }
        
        void draw(){
            
            ofPushMatrix();
            
            if(bDraw){
                // room                
                {
                    ofPushMatrix();
                    ofRotateXDeg(orientation.get().x);
                    ofRotateZDeg(orientation.get().z);
                    ofRotateYDeg(orientation.get().y);
                    
                    // room
                    ofSetColor(150);
                    roomVbo.draw();
          
                    // sun trj
                    ofSetColor(255, 0, 0);
                    ofDrawSphere(sunOnTheWall, 3);
                    sunWallPath.drawVertices();
                    
                    // moon trj
                    ofSetColor(255, 250, 0);
                    ofDrawSphere(moonOnTheWall, 3);
                    moonWallPath.drawVertices();
                    
                    ofPopMatrix();
                }
                
                //ofSetColor(150);
                //box.drawWireframe();
            }
            

            ofPopMatrix();
        }

        ofBoxPrimitive box;
        
        ofVboMesh roomVbo;
        ofVboMesh sunWallPath;
        ofVboMesh moonWallPath;

        ofParameter<bool> bDraw{"draw room", false};
        ofParameter<bool> bDrawCompass{"draw compass", false};
        ofParameter<float> width{"width (cm)", 750, 100, 1000};
        ofParameter<float> height{"height (cm)", 350, 100, 1000};
        ofParameter<float> depth{"depth (cm)", 900, 100, 1000};
        ofParameter<vec3>  orientation{"orientation", vec3(0), -vec3(180), vec3(180)};
        ofParameterGroup grp{"Room", bDraw, bDrawCompass, width, height, depth, orientation};

        vec3 sunOnTheWall;
        vec3 moonOnTheWall;


        ofEventListeners cbs;
    };
}
