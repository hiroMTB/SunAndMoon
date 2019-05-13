//
//  Sun.h
//

#pragma once

#include "ofMain.h"
#include "ofxRaycaster.h"
#include "ofxSunCalc.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/LocalDateTime.h"
#include "Poco/Timespan.h"

using namespace glm;
//using namespace ofxraycaster;

namespace sunandmoon{
    
    class Sun{
        
    public:
        Sun(){
            vec3 direction = glm::vec3(0,1,0);
            vec3 origin = glm::vec3(0, 0, 0);
            ray.setup(origin, direction);
            int w = ofGetScreenWidth();
            timeline.allocate(w-20-100, 32);
        }
        
        ~Sun(){
            clear();
        }
        
        void update(const Poco::DateTime & utcDate, const float lat, const float lng, int tz){
            
            sunpos = ofxSunCalc::getSunPosition(utcDate, lat, lng);
            sunpos.azimuth += PI;   // something wrong in ofxSunCalc
            info = ofxSunCalc::getDayInfo(utcDate, lat, lng, false);

            Poco::Timespan dateOffset(0,tz,0,0,0);
            info.sunrise.start += dateOffset;
            info.sunrise.end += dateOffset;
            info.sunset.start += dateOffset;
            info.sunset.end += dateOffset;
            info.dawn += dateOffset;
            info.dusk += dateOffset;

            brightness = ofxSunCalc::getSunBrightness(info, utcDate+dateOffset);

            ofxSunCalc::drawSimpleDayInfoTimeline(timeline, info);
            
            altitude = sunpos.altitude * RAD_TO_DEG;
            azimuth  = sunpos.azimuth * RAD_TO_DEG;
            dawn = ofxSunCalc::dateToTimeString(info.dawn);
            rise = ofxSunCalc::dateToTimeString(info.sunrise.start);
            noon = ofxSunCalc::dateToTimeString(info.transit);
            set = ofxSunCalc::dateToTimeString(info.sunset.end);
            dusk = ofxSunCalc::dateToTimeString(info.dusk);
            
            // calc
            float az = sunpos.azimuth;
            float alt = sunpos.altitude;
            
            sunPos1 = glm::rotate(north, az, -up);
            vec3 rotAxis = glm::rotate(east, az, -up);
            sunPos2 = glm::rotate(sunPos1, alt, rotAxis);  // this is the position
            sunPos3 = vec3(sunPos2.x, 0, sunPos2.z);       // this is projected position (z=0 plane)
            sunPosVec = sunPos2;
            
            // ray
            ray.setDirection(glm::normalize(sunPos2));        
            
            // vbo
            const vec3 & origin = ray.getOrigin();
            trj.addVertex(sunPos2*rad + origin);
        }
        
        void draw(){
            
            if(bDrawSun){
                const vec3 & origin = ray.getOrigin();
                const vec3 & pos = sunPosVec.get();
                // guide lines
                ofSetColor(200);
                ofDrawLine(origin, sunPos1*rad+origin);
                ofDrawLine(pos*rad+origin, sunPos3*rad+origin);
                
                ofSetColor(250, 0, 0);
                ofDrawLine(origin, pos*rad+origin);
                
                // sun on the sky
                ofFill();
                ofDrawSphere(pos*rad + origin, sunSize);
                
                // sun traj on the sky
                ofSetColor(250, 0, 0);
                trj.drawVertices();
            }
        }
        
        void clear(){
            trj.clear();
        }
        
        SunCalcDayInfo info;
        SunCalcPosition sunpos;
        
        ofxraycaster::Ray ray;
        
        ofVboMesh trj;
        ofFbo timeline;

        ofParameter<bool > bDrawSun{"draw sun", true};
        ofParameter<float> brightness{"sun brightness", 0, 0, 100};
        ofParameter<vec3> sunPosVec{"sun position (vec)", vec3(0), vec3(-1), vec3(1) };
        ofParameter<float> altitude{"sun altitude", 0, -180, 180};
        ofParameter<float> azimuth{"sun azimuth", 0, -180, 180};
        ofParameter<string> dawn{"dawn", "n.a."};
        ofParameter<string> rise{"sunrise", "n.a."};
        ofParameter<string> noon{"solar noon", "n.a."};
        ofParameter<string> set{"sunset", "n.a."};
        ofParameter<string> dusk{"dusk", "n.a."};
        ofParameterGroup grp{"Sun", bDrawSun, brightness, sunPosVec, azimuth, altitude, dawn, rise, noon, set, dusk};
        
    private:
        vec3 sunPos1, sunPos2, sunPos3;     // gudie line, sunPos2 = sunPosVec
        const float rad{1000};
        const float sunSize{10};
        const vec3 up{0, 1, 0};
        const vec3 north{0, 0, 1};
        const vec3 east{-1, 0, 0};
        const vec3 south{0, 0, -1};
        const vec3 west{1, 0, 0};
    };
}
