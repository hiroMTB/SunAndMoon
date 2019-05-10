//
//  Earth.h
//

#pragma once

#include "ofMain.h"
#include "City.h"

using namespace glm;
namespace sunandmoon{
    
    class Earth{

    public:

        Earth(){
            ofDisableArbTex();
            //tex.setAlphaMask(mask);
            bool ok = ofLoadImage(tex, "img/Albedo.jpg"); // "img/GDEM-10km-BW_tr.png");
            if(!ok){
                ofLogError() << "can not find earth texture";
            }

            sphere.set(earthRadius, 64);
            sphere.setScale(1);
            sphere.setPosition(vec3(0, 0, 0));
            //sphere.setOrientation(vec3(0, 180, 0));
        }
        
        ~Earth(){
            
        }
                
        void draw(){

            ofPushStyle();
            ofPushMatrix();
            ofTranslate(0, -earthRadius-200);

            // PIN
            {
                ofSetColor(255, 0, 150);
                ofDrawLine(0,earthRadius+500, 0, earthRadius+700);
                ofFill();
                ofDrawSphere(vec3(0,earthRadius+700,0), 15);
                string label = city.cityName.get() + " / " + city.countryName.get();
                ofDrawBitmapString(label, 0, earthRadius+700+50);
            }

            ofRotateXDeg(90-city.lat);
            ofRotateYDeg(-city.lng);

            ofDrawAxis(earthRadius);

            if(bDrawEarth){
                // earth
                ofEnableAlphaBlending();
                ofSetColor(255);
                tex.bind();
                sphere.draw();
                tex.unbind();

                // Red line
                ofPushMatrix();
                ofRotateXDeg(90);
                ofSetColor(255, 0, 0 );
                ofNoFill();
                ofDrawCircle(0, 0, earthRadius*1.05);
                ofPopMatrix();
                
                // Pole
                ofSetColor(0, 0, 255);
                ofDrawLine(0, -earthRadius*1.2, 0, earthRadius*1.2);
            }

            // All city
            if(city.bDrawCity){
                ofPushMatrix();
                ofScale(earthRadius);
                ofSetColor(255, 200);
                city.draw();
                ofPopMatrix();
            }

            ofPopStyle();
            ofPopMatrix();
        }

        ofTexture tex;
        ofSpherePrimitive sphere;
        City city;

        ofParameter<float> earthRadius{"Earth radius (cm)", 10000, 100, 6378136.6 * 100};
        ofParameter<bool> bDrawEarth{"draw earth", true};
        ofParameterGroup grp{"Earth", bDrawEarth, earthRadius, city.grp};

    };
}
