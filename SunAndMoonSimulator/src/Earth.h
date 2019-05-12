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
            string p = ofToDataPath("img/Albedo.jpg", true);
            bool ok = ofLoadImage(tex, p); // "img/GDEM-10km-BW_tr.png");
            if(!ok){
                ofLogError() << "Can not find CityData json file : " << p;
            }

            sphere.set(1, 64);
            sphere.setScale(1);
            sphere.setPosition(vec3(0, 0, 0));
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
                ofPushMatrix();
                ofScale(earthRadius);
                tex.bind();
                sphere.draw();
                tex.unbind();
                ofPopMatrix();

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

        ofParameter<bool> bDrawSky{"draw sky", false};
        ofParameter<float> earthRadius{"Earth radius (cm)", 10000, 100, 6378136.6 * 100};
        ofParameter<bool> bDrawEarth{"draw earth", true};
        ofParameterGroup grp{"Earth", bDrawEarth, bDrawSky, earthRadius, city.grp};

    };
}
