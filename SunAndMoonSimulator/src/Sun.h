//
//  Sun.h
//

#pragma once

#include "ofMain.h"
#include "ofxRaycaster.h"
#include "ofxSunCalc.h"
#include "Poco/DateTime.h"
#include "Poco/Timespan.h"
#include "Constants.h"
#include "Planet.h"

using namespace glm;

namespace sunandmoon{
    
    class Sun : public Planet{
        
    public:
        Sun(){
            color = ofColor(250, 0, 0);
            grp.setName("Sun");
        }
        
        void update(const Poco::DateTime & utcDate, const float lat, const float lng, int tz){
            
            CalcPosition calcPos = ofxSunCalc::getSunPosition(utcDate, lat, lng);
            updateCommon(calcPos);
            
            // update info
            SunCalcDayInfo info = ofxSunCalc::getDayInfo(utcDate, lat, lng, true);
            Poco::Timespan dateOffset(0,tz,0,0,0);
            info.sunrise.start += dateOffset;
            info.sunrise.end += dateOffset;
            info.sunset.start += dateOffset;
            info.sunset.end += dateOffset;
            info.dawn += dateOffset;
            info.dusk += dateOffset;
            ofxSunCalc::drawSimpleDayInfoTimeline(timeline, info);
            dawn = ofxSunCalc::dateToTimeString(info.dawn);
            rise = ofxSunCalc::dateToTimeString(info.sunrise.start);
            noon = ofxSunCalc::dateToTimeString(info.transit);
            set = ofxSunCalc::dateToTimeString(info.sunset.end);
            dusk = ofxSunCalc::dateToTimeString(info.dusk);
            
            // this is not scientific
            brightness = ofxSunCalc::getSunBrightness(info, utcDate+dateOffset);
        }        
    };
}
