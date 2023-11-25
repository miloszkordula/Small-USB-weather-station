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
public:
    int update(const char* comPort);
    double getTemperature();
    double getPressure();
    double getHumidity();
    double getDewPoint();
    std::string getTime();
};

