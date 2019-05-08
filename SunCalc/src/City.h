//
//  City.h
//


#pragma once

#include <string>
#include <vector>

#include "ofMain.h"

using namespace std;

namespace sunandmoon{
    
    class City{

    public:
        City(string _name, string _country, float _lat, float _lng)
        :name(_name), country(_country), lat(_lat), lng(_lng){}
        
        string name;
        string country;
        float lat;
        float lng;
    };
    
    
    class CityData{
        
    public:
        
        CityData(){
        
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
                }
            }
        }
        
        vector<City> data;
        
    };
}

