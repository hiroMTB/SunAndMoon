#include "ofApp.h"

void ofApp::calcIntersectionWall(){
    
    {
        // Sun and Room
        glm::vec3 bari;
        glm::vec3 normal;
        ofxraycaster::Ray & ray = sun.ray;
        bool bHit = (ray.intersectsPrimitive(room.box, bari, normal));
        if(bHit){
            vec3 intersection = ray.getOrigin() + ray.getDirection() * bari.z;
            room.addSunTrace(intersection);
        }
    }
    
    {
        // Moon and Room
        ofxraycaster::Ray & ray = moon.ray;
        glm::vec3 bari;
        glm::vec3 normal;
        bool bHit = (ray.intersectsPrimitive(room.box, bari, normal));
        if(bHit){
            vec3 intersection = ray.getOrigin() + ray.getDirection() * bari.z;
            room.addMoonTrace(intersection);
        }
    }
}

void ofApp::calcIntersectionWindow( vector<Ray> & rays, vec3 dir, float planetHeight,
                                      ofColor col){
    
    bool bPlanetIsHigh = (planetHeight > 0);
    if(bPlanetIsHigh){
        float maxLen = 500;
        unsigned int maxRef = 1;
        const mat4 & mat = window.plane.getGlobalTransformMatrix();
        ofMesh & m = window.plane.getMesh();
        
        for(int r=0; r<window.nRow; r++){
            for(int c=0; c<window.nCol; c++){
                int index = c + window.nCol * r;
                if(index < rays.size()){
                    vec4 origin4 = mat * vec4(m.getVertex(index),1);
                    vec3 origin = vec3(origin4) + dir;
                    rays[index].setOrigin(origin);
                    rays[index].setDirection(dir);
                    rays[index].setMaxLength(maxLen);
                    rays[index].setMaxReflectionNum(maxRef);
                    rays[index].intersectsPrimitiveMultiCast(room.box);
                }
            }
        }
    }else{
        for(int r=0; r<window.nRow; r++){
            for(int c=0; c<window.nCol; c++){
                int index = c + window.nCol * r;
                if(index < rays.size()){
                    rays[index].clearVbo();
                }
            }
        }
    }
}

