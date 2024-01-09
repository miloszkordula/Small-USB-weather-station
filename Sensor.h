#pragma once
#include <string>
#include <sstream>
#include <time.h>
class Sensor
{ 
    double temperature = 20;
    double pressure = 1000;
    double humidity = 50;
    double dewPoint = 10;
    std::string time;
    std::tm localTime;

    double tempA = 1;
    double tempB = -4;
    double humiA = 1;
    double humiB = 0;
    double presA = 1;
    double presB = 0;
    double dewpA = 1;
    double dewpB = 0;

public:
    int update(const char* comPort);
    
    double getTemperature();
    double getPressure();
    double getHumidity();
    double getDewPoint();
    std::string getTime();
    
    double getTempA();
    double getTempB();
    double getHumiA();
    double getHumiB();
    double getPresA();
    double getPresB();
    double getDewpA();
    double getDewpB();

    void setCalibration(double tempA, double tempB, double humiA,
        double humiB, double presA, double presB, double dewpA, double dewpB);
    void restoreCalibration();

    void saveCalibration(std::string calibration);
    
   
};

