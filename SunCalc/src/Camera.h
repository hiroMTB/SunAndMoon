//
//  Camera.h
//

#pragma once

#include "ofMain.h"

namespace module{
    
    class Camera{
        
        public:
        
        Camera(){};
        ~Camera(){};
        
        void begin(){
            bOrtho ? cam.enableOrtho() : cam.disableOrtho();
            cam.setVFlip(bVflip);
            cam.setNearClip(near);
            cam.setFarClip(far);
            cam.setFov(fov);
            cam.begin();
        }
        
        void end(){
            cam.end();
        }
        
        ofParameter<bool> bOrtho{"Ortho", false};
        ofParameter<bool> bVflip{"V flip", false};
        ofParameter<float> near{"Near", 0, 0, 1000};
        ofParameter<float> far{"Far", 0, 0, 10000};
        ofParameter<float> fov{"fov", 1, 1, 360};
        ofParameterGroup grp{"Camera", bOrtho, bVflip, near, far, fov};
        
        ofEasyCam & getCam(){
            return cam;
        }
        
        private:
        ofEasyCam cam;
        
    };
}

