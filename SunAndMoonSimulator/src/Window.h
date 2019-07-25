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
        
        Window(){}
        
        void setup(ofNode & parent){
            
            plane.setParent(parent);
            
            cbs.push( nCol.newListener([=](int &){ this->reset();} ) );
            cbs.push( nRow.newListener([=](int &){ this->reset();} ) );
            
            cbs.push( width.newListener([=](int &){ this->resetPlane();} ) );
            cbs.push( height.newListener([=](int &){ this->resetPlane();} ) );
            cbs.push( pos.newListener([=](vec3 &){ this->resetPlane();} ) );
            cbs.push( orientation.newListener([=](vec3 & v){ this->resetPlane();} ) );
            
            reset();
        }

        ~Window(){
            clear();
        }
        
        void reset(){
            clear();
            resetPlane();
            resetRay();
        }
        
        void resetPlane(){
            plane.set(width, height, nCol, nRow);
            plane.setPosition(pos);
            plane.setOrientation(orientation);
        }
        
        void resetRay(){
            clear();
            int nRay = nCol * nRow;
            sunRays.resize(nRay);
            moonRays.resize(nRay);
            bNeedReset = false;
        }
        
        void drawWindow(){
            if(bDraw){
                ofSetColor(200);
                plane.drawWireframe();
            }
        }
        
        void drawSunRays(){
            ofSetColor(250, 0, 0);
            for(auto & r : sunRays){
                r.getTrack().draw();
            }
        }
        
        void drawMoonRays(){
            ofSetColor(250, 250, 0);
            for(auto & r : moonRays){
                r.getTrack().draw();
            }
        }
        
        void clear(){
            sunRays.clear();
            moonRays.clear();
        }
        
        ofParameter<bool> bDraw{"draw window", true};
        ofParameter<vec3> pos{"position", vec3(0), vec3(-5000), vec3(5000)};  // relative to room origin
        ofParameter<vec3> orientation{"orientation", vec3(0), vec3(-180), vec3(180)};
        ofParameter<int> width{"Width", 100, 10, 3000};
        ofParameter<int> height{"Height", 100, 10, 3000};
        ofParameter<int> nCol{"Number of Ray colum", 10, 1, 100};
        ofParameter<int> nRow{"Number of Ray row", 10, 1, 100};
        ofParameterGroup grp{"Window", bDraw, pos, orientation, width, height, nCol, nRow};
        
        ofPlanePrimitive plane;
        vector<Ray> sunRays;
        vector<Ray> moonRays;

        ofEventListeners cbs;

    private:
        
        bool bNeedReset = true;
    };
}
