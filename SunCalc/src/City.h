//
//  City.h
//


#pragma once

#include "ofMain.h"

using namespace std;
using namespace glm;

namespace sunandmoon{
    
    struct CityData{

    public:
        CityData(string _name, string _country, float _lat, float _lng)
        :name(_name), country(_country), lat(_lat), lng(_lng){}
        
        string name;
        string country;
        float lat;
        float lng;
    };
    
    
    class City{
        
    public:
        
        City(){
        
            ofFile file("json/worldcities.csv");
            if(file.exists()){
                ofBuffer buffer(file);
                for (ofBuffer::Line it = ++buffer.getLines().begin(), end = buffer.getLines().end(); it != end; ++it) {
                    string line = *it;
                    vector<string> words = ofSplitString(line, ",");
                    string cityName = words[0];
                    string countryName = words[4];
                    float latitude = ofToFloat(words[2]);
                    float longitude = ofToFloat(words[3]);
                    data.emplace_back( cityName, countryName, latitude, longitude );

                    vec3 v(0,0,-1);
                    v = glm::rotateX(v, ofDegToRad(latitude));
                    v = glm::rotateY(v, ofDegToRad(longitude));
                    cityVbo.addVertex(v);
                    cityVbo.addVertex(v*1.0025);

                    nCity++;
                }
            }

            cityId.setMax( nCity );
            cityVbo.setMode(OF_PRIMITIVE_LINES);
        }
        
        void draw(){
            cityVbo.draw();
        }

        void goToNextCity(){
            setCityId(cityId+1);
        }

        void goToPrevCity(){
            setCityId(cityId-1);
        }

        void setCityId(int id){
            cityId = id % nCity;
            CityData & c = data[cityId];
            cityName = c.name;
            countryName = c.country;
            lat = c.lat;
            lng = c.lng;
        }

        ofParameter<bool> bDrawCity{"draw city", false};
        ofParameter<int> cityId{"City Id", 0, 0, 10000};
        ofParameter<string> cityName{"City Name", "n.a."};
        ofParameter<string> countryName{"Country Name", "n.a."};
        ofParameter<float> lat{"latitude", 52.52, -90, 90};
        ofParameter<float> lng{"longtitude", 13.40, -180, 180};

        ofParameterGroup grp{"City", bDrawCity, cityId, cityName, countryName, lat, lng};

        vector<CityData> data;
        ofVboMesh cityVbo;

    private:
        int nCity = 0;

    };
}