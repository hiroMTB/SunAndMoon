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
            ofLogNotice() << "Window constructed";

            cbs.push( nCol.newListener([=](int &){ this->requestReset();} ) );
            cbs.push( nRow.newListener([=](int &){ this->requestReset();} ) );
            cbs.push( width.newListener([=](int &){ this->requestReset();} ) );
            cbs.push( height.newListener([=](int &){ this->requestReset();} ) );
            cbs.push( pos.newListener([=](vec3 &){ this->requestReset();} ) );
            cbs.push( orientation.newListener([=](vec3 & v){ this->requestReset();} ) );
        }

        ~Window(){
            ofLogNotice() << "Window destucted";
            clear();
        }
        
        void requestReset(){
            bNeedReset = true;
        }
        
        void reset(){
            clear();

            plane.disableNormals();
            plane.set(width, height, nCol, nRow);
            plane.setPosition(pos);
            plane.setOrientation(orientation);
            
            int nRay = nCol * nRow;
            sunRays.resize(nRay);
            moonRays.resize(nRay);
            
            // vbo
            sunRayVbo.setMode(OF_PRIMITIVE_LINES);
            moonRayVbo.setMode(OF_PRIMITIVE_LINES);
            sunRayVbo.clear();
            sunRayVbo.getVertices().resize(nRay*4);
            sunRayVbo.getColors().resize(nRay*4);
            
            moonRayVbo.clear();
            moonRayVbo.getVertices().resize(nRay*4);
            moonRayVbo.getColors().resize(nRay*4);
            
            bNeedReset = false;
            
            ofLogNotice() << "Window object reset";
            ofLogNotice() << "nCol = " << nCol;
            ofLogNotice() << "nRow = " << nRow;
            ofLogNotice() << "SunRays size = " << sunRays.size();
            ofLogNotice() << "sunVbo  size = " << sunRayVbo.getVertices().size() << endl;
        }
        
        void draw(){
            
            if(bNeedReset){
                reset();
                ofLogVerbose() << "reset" << endl;
            }
            
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
        vector<Ray> sunRays;
        vector<Ray> moonRays;

        ofVboMesh sunRayVbo;
        ofVboMesh moonRayVbo;

        ofEventListeners cbs;

    private:
        
        bool bNeedReset = true;

        
    };
}
