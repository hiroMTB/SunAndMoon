//
//  Window.h
//

#pragma once

#include "ofMain.h"
#include "ofxRaycaster.h"

using namespace glm;
using namespace ofxraycaster;

namespace sunandmoon{
    
    class Window{

    public:
        
        Window(){
            sunRayVbo.setMode(OF_PRIMITIVE_LINES);
            moonRayVbo.setMode(OF_PRIMITIVE_LINES);
            
            cbs.push( nCol.newListener([&](int & i){ resetRay();} ) );
            cbs.push( nRow.newListener([&](int & i){ resetRay();} ) );
            cbs.push( width.newListener([&](int & i){ resetPlane();} ) );
            cbs.push( height.newListener([&](int & i){ resetPlane();} ) );
            cbs.push( pos.newListener([&](vec3 & v){ resetPlane();} ) );
            cbs.push( orientation.newListener([&](vec3 & v){ resetPlane();} ) );                        
        }

        ~Window(){
            clear();
        }
        
        void resetPlane(){
            plane.set(width, height, nCol, nRow);
            plane.setPosition(pos);
            plane.setOrientation(orientation);
        }
        
        void resetRay(){
           
            clear();
            resetPlane();
            
            int nRay = nCol * nRow;
            sunRays.resize(nRay);
            moonRays.resize(nRay); 
        }
        
        void draw(){
            plane.drawWireframe();
            
            ofSetColor(255, 0, 0);
            sunRayVbo.draw();
            moonRayVbo.draw();
        }
        
        void clear(){
            sunRayVbo.clear();
            moonRayVbo.clear();
            sunRays.clear();
            moonRays.clear();
        }
        
        ofParameter<vec3> pos{"position", vec3(0), vec3(-5000), vec3(5000)};  // relative to room origin
        ofParameter<vec3> orientation{"orientation", vec3(0), vec3(-180), vec3(180)};
        ofParameter<int> width{"Width", 100, 10, 3000};
        ofParameter<int> height{"Height", 100, 10, 3000};
        ofParameter<int> nCol{"Number of Ray colum", 10, 1, 100};
        ofParameter<int> nRow{"Number of Ray row", 10, 1, 100};
        ofParameterGroup grp{"Window", pos, orientation, width, height, nCol, nRow};
        
        ofPlanePrimitive plane;
        vector<shared_ptr<Ray>> sunRays;
        vector<shared_ptr<Ray>> moonRays;

        ofVboMesh sunRayVbo;
        ofVboMesh moonRayVbo;

        ofEventListeners cbs;
    };
}
