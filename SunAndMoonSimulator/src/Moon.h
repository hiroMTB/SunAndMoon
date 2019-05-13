//
//  Moon.h
//

#pragma once

namespace sunandmoon{
    
    class Moon{

    public:
        
        Moon(){
            vec3 direction = glm::vec3(0,1,0);
            vec3 origin = glm::vec3(0, 0, 0);
            ray.setup(origin, direction);
        }
        
        ~Moon(){
            clear();
        }
        
        void update(const Poco::DateTime & utcDate, const float lat, const float lng, int tz){

            moonpos = ofxSunCalc::getMoonPosition(utcDate, lat, lng);
            moonpos.azimuth += PI;   // something wrong in ofxSunCalc

            float az = moonpos.azimuth;
            float alt = moonpos.altitude;
            
            pos1 = glm::rotate(north, az, -up);
            vec3 rotAxis = glm::rotate(east, az, -up);
            pos2 = glm::rotate(pos1, alt, rotAxis);  // this is the position
            pos3 = vec3(pos2.x, 0, pos2.z);       // this is projected position (z=0 plane)
            posVec = pos2;
            
            
            ray.setDirection(glm::normalize(pos2));
            
            // vbo
            const vec3 & origin = ray.getOrigin();
            trj.addVertex(pos2*rad+origin);
        }
        
        void draw(){
            
            if(bDrawMoon){
                
                const vec3 & origin = ray.getOrigin();
                const vec3 & pos = posVec.get();
                
                // guide lines
                ofSetColor(200);
                ofDrawLine(origin, pos1*rad+origin);
                ofDrawLine(pos2*rad+origin, pos3*rad+origin);
                
                ofSetColor(250, 250, 0);
                ofDrawLine(origin, pos2*rad+origin);
                
                // moon on the sky
                ofFill();
                ofDrawSphere(pos2*rad+origin, moonSize);
                
                // traj on the sky
                ofSetColor(250, 250, 0);
                trj.drawVertices();
            }
        }
        
        void clear(){
            trj.clear();
        }
        
        // moon
        ofParameter<bool > bDrawMoon{"draw moon", true};
        ofParameter<float> brightness{"brightness", 0, 0, 100};
        ofParameter<vec3> posVec{"position (vec)", vec3(0), vec3(-1), vec3(1) };
        ofParameterGroup grp{"Moon", bDrawMoon, brightness, posVec};
        
        ofVboMesh trj;
        ofxraycaster::Ray ray;        
        MoonCalcPosition moonpos;
        
        
    private:
        vec3 pos1, pos2, pos3;     // gudie line, sunPos2 = sunPosVec
        const float rad{1000};
        const float moonSize{10};
        const vec3 up{0, 1, 0};
        const vec3 north{0, 0, 1};
        const vec3 east{-1, 0, 0};
        const vec3 south{0, 0, -1};
        const vec3 west{1, 0, 0};
    };
}
