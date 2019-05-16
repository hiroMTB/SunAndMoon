//
//  ofxSunCalc.h
//
//  Created by Adam Hinshaw on 30/04/2015.
//  Port of js @ http://suncalc.net/
//

#ifndef __ofxSunCalc__
#define __ofxSunCalc__

/*
 (c) 2011-2014, Vladimir Agafonkin
 SunCalc is a JavaScript library for calculating sun/mooon position and light phases.
 https://github.com/mourner/suncalc
 */

#include "ofMain.h"

#include "Poco/DateTime.h"
#include "Poco/DateTimeFormatter.h"

typedef struct {
    double azimuth;
    double altitude;
    double distance;            // only for moon
    double parallacticAngle;    // only for moon
} CalcPosition;

typedef struct {
    Poco::DateTime start;
    Poco::DateTime end;
} SunCalcDateRange;

typedef struct {
    bool isSet = false;
    
    SunCalcDateRange morningTwilightAstronomical;
    SunCalcDateRange morningTwilightNautical;
    SunCalcDateRange morningTwilightCivil;
    
    SunCalcDateRange nightTwilightAstronomical;
    SunCalcDateRange nightTwilightNautical;
    SunCalcDateRange nightTwilightCivil;
    
} SunCalcDayInfoExtended;

typedef struct {
    
    Poco::DateTime dawn;
    SunCalcDateRange sunrise;
    Poco::DateTime transit;
    SunCalcDateRange sunset;
    Poco::DateTime dusk;
    
    SunCalcDayInfoExtended extended;
    
    double lat;
    double lon;
    
} SunCalcDayInfo;

class ofxSunCalc {
    
public:
    
    ofxSunCalc();
    
    static double dateToJulianDate( const Poco::DateTime & date );
    static Poco::DateTime julianDateToDate( double j );
    static int getJulianCycle( double J, double lw );
    static double getApproxSolarTransit( double Ht, double lw, double n );
    static double getSolarMeanAnomaly( double Js );
    static double getEquationOfCenter( double M );
    static double getEclipticLongitude( double M, double C );
    static double getSolarTransit( double Js, double M, double Lsun );
    static double getSunDeclination( double Lsun );
    static double getRightAscension( double Lsun );
    static double getSiderealTime( double J, double lw );
    static double getAzimuth( double th, double a, double phi, double d );
    static double getAltitude( double th, double a, double phi, double d );
    static double getHourAngle( double h, double phi, double d );
    static double getSunsetJulianDate( double w0, double M, double Lsun, double lw, double n );
    static double getSunriseJulianDate( double Jtransit, double Jset );
    static CalcPosition getSunPosition( const Poco::DateTime & date, double lat, double lon );
    static CalcPosition getSunPosition( double J, double lw, double phi );
    static CalcPosition getMoonPosition( const Poco::DateTime & date, double lat, double lon);
    static SunCalcDayInfo getDayInfo( const Poco::DateTime & date, double lat, double lon, bool detailed = false );
    static string infoToString(const SunCalcDayInfo & info, bool min = true);    
    static string dateToString(const Poco::DateTime & date);
    static string dateToDateString(const Poco::DateTime & date);
    static string dateToTimeString(const Poco::DateTime & date);
    static float getSunBrightness(SunCalcDayInfo & info, const Poco::DateTime time);
    static void drawSimpleDayInfoTimeline(ofFbo & target, SunCalcDayInfo & info);
    static void drawExtendedDayInfoTimeline(ofFbo & target, SunCalcDayInfo & info);
    
private:
    static double rightAscension(double l, double b);
    static double declination(double l, double b);
    static double azimuth(double H, double phi, double dec);
    static double altitude(double H, double phi, double dec);
    static double siderealTime(double d, double lw);
    static double astroRefraction(double h);

private:
    
    constexpr static double J1970 = 2440588;
    constexpr static double J2000 = 2451545;
    constexpr static double dayMs = 1000 * 60 * 60 * 24;
    constexpr static double deg2rad = PI / 180.0;
    constexpr static double M0 = 357.5291 * deg2rad;
    constexpr static double M1 = 0.98560028 * deg2rad;
    constexpr static double J0 = 0.0009;
    constexpr static double J1 = 0.0053;
    constexpr static double J2 = -0.0069;
    constexpr static double C1 = 1.9148 * deg2rad;
    constexpr static double C2 = 0.0200 * deg2rad;
    constexpr static double C3 = 0.0003 * deg2rad;
    constexpr static double P = 102.9372 * deg2rad;
    constexpr static double e = 23.4397 * deg2rad; // obliquity of the Earth
    constexpr static double th0 = 280.1600 * deg2rad;
    constexpr static double th1 = 360.9856235 * deg2rad;
    constexpr static double h0 = -0.833 * deg2rad; //sunset angle
    constexpr static double d0 = 0.53 * deg2rad; //sun diameter
    constexpr static double h1 = -6 * deg2rad; //nautical twilight angle
    constexpr static double h2 = -12 * deg2rad; //astronomical twilight angle
    constexpr static double h3 = -18 * deg2rad; //darkness angle
    
};

#endif /* defined(__ofxSunCalc__) */
