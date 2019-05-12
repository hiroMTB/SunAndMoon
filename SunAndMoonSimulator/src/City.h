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
        CityData(int _cityId, string _name, string _country, float _lat, float _lng)
        :cityId(_cityId), name(_name), country(_country), lat(_lat), lng(_lng){}

        int cityId;
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
                    string cityName = words[1];
                    string countryName = words[4];
                    float latitude = ofToFloat(words[2]);
                    float longitude = ofToFloat(words[3]);
                    data.emplace_back( nCity, cityName, countryName, latitude, longitude );

                    vec3 v(0,0,-1);
                    v = glm::rotateX(v, ofDegToRad(latitude));
                    v = glm::rotateY(v, ofDegToRad(longitude));
                    cityVbo.addVertex(v);
                    cityVbo.addVertex(v*1.0005);

                    nCity++;
                }
            }else{
                ofLogError() << "Can not find CityData json file : " << file.getAbsolutePath();
            }

            cityId.setMax( nCity );
            cityVbo.setMode(OF_PRIMITIVE_LINES);
            
            cbs.push( cityId.newListener([&](int & i){setCityId(cityId);}));
            cbs.push( cityName.newListener([&](string & s){ setCityByName(s);}));
        }
        
        void draw(){
            ofSetColor(255,0,150);
            cityVbo.draw();
        }

        void goToNextCity(){
            setCityId(cityId+1);
        }

        void goToPrevCity(){
            setCityId(cityId-1);
        }

        void setCityId(int id){
            if(nCity == 0){
                ofLogError() << "CityData is empty" << endl;
                setError();
                return;
            }
            cityId = id % nCity;
            CityData & c = data[cityId];
            cityName = c.name;
            countryName = c.country;
            lat = c.lat;
            lng = c.lng;
        }
        
        void setCityByName(string & name){
            vector<CityData>::iterator itr = std::find_if(data.begin(),data.end(),[&](const CityData & c){ return (c.name == name);});
            if(itr != data.end()){
                setCityId(itr->cityId);
            }else{
                setError();
            }
        }

        void setError(){
            cityName = "Can not find city name";
            countryName = "Can not find city name";
            cityId = -1;
        }
        
        ofParameter<bool> bDrawCity{"draw city", false};
        ofParameter<int> cityId{"City Id", 6279, -1, 10000};
        ofParameter<string> cityName{"City", "Berlin"};
        ofParameter<string> countryName{"Country", "Germany"};
        ofParameter<float> lat{"latitude", 52.52, -90, 90};
        ofParameter<float> lng{"longtitude", 13.40, -180, 180};
        ofParameter<int> timezone{"Estimated time zone", 0, -12, 12};
        ofParameterGroup grp{"City", bDrawCity, cityId, cityName, countryName, lat, lng, timezone};

        vector<CityData> data;
        ofVboMesh cityVbo;

    private:
        int nCity = 0;
        
        ofEventListeners cbs;

    };
}
