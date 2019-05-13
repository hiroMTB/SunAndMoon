//
//  Moon.h
//

#pragma once

#include "ofMain.h"
#include "ofxRaycaster.h"
#include "ofxSunCalc.h"
#include "Poco/DateTime.h"
#include "Constants.h"
#include "Planet.h"

using namespace glm;

namespace sunandmoon{
    
    class Moon : public Planet{

    public:
        
        Moon(){
            color = ofColor(250, 250, 0);
            grp.setName("Moon");
        }
        
        void update(const Poco::DateTime & utcDate, const float lat, const float lng, int tz){

            CalcPosition calcPos = ofxSunCalc::getMoonPosition(utcDate, lat, lng);
            updateCommon(calcPos);
        }
    };
}
