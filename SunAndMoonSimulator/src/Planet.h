//
//  Planet.h
//
//

#pragma once

namespace sunandmoon{
    
    class Planet{
    public:
        
        Planet(){
            vec3 direction = glm::vec3(0,1,0);
            vec3 origin = glm::vec3(0, 0, 0);
            ray.setup(origin, direction);
        }
        
        ~Planet(){
            clear();
        }
        
        void updateCommon( CalcPosition & calcPos ){
            calcPos.azimuth += PI;
            float az = calcPos.azimuth;
            float alt = calcPos.altitude;
            azimuth = az * RAD_TO_DEG;
            altitude = alt * RAD_TO_DEG;
            dir = glm::rotate(north, az, -up);
            vec3 rotAxis = glm::rotate(east, az, -up);
            pos = glm::rotate(dir, alt, rotAxis);
            
            ray.setDirection(glm::normalize(pos));
            
            // vbo
            const vec3 & origin = ray.getOrigin();
            float rad = radius.get();
            trj.addVertex(pos * rad + origin);
        }
        
        void draw(){
            
            if(bDraw){                
                const vec3 & origin = ray.getOrigin();
                float rad = radius.get();
                vec3 posXZ = vec3(pos.x, origin.y, pos.z);
                vec3 pPos = pos * rad + origin;
                
                // guide lines
                ofSetColor(200);
                ofDrawLine(origin, dir*rad+origin);
                ofDrawLine(pPos, posXZ*rad+origin); // vertical
                
                ofSetColor(color);
                ofDrawLine(pPos, origin);
                
                // Planet on the sky
                ofFill();
                ofDrawSphere(pPos, size);
                
                // traj on the sky
                trj.drawWireframe();
            }
        }
        
        void clear(){
            trj.clear();
        }
        
        ofParameter<bool> bDraw{"draw", true};
        ofParameter<float> radius{"radius", 1000, 10, 5000};
        ofParameter<float> brightness{"brightness", 0, 0, 1};
        ofParameter<float> altitude{"altitude", 0, -180, 180};
        ofParameter<float> azimuth{"azimuth", 0, -180, 180};
        ofParameter<string> dawn{"dawn", "n.a."};
        ofParameter<string> rise{"rise", "n.a."};
        ofParameter<string> noon{"noon", "n.a."};
        ofParameter<string> set{"set", "n.a."};
        ofParameter<string> dusk{"dusk", "n.a."};
        ofParameterGroup grp{"Planet", bDraw, radius, brightness, azimuth, altitude, dawn, rise, noon, set, dusk};
        
        vec3 pos;   // 3d
        vec3 dir;   // 2d
        const float size{10};

        ofxraycaster::Ray ray;
        ofVboMesh trj;
        ofFbo timeline;
        
        ofColor color{255};
    };
}
