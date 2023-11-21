#pragma once
#include <string>
#include <sstream>
class Sensor
{ 
    double temperature = 20;
    double pressure = 1000;
    double humidity = 50;
    double dewPoint = 10;
public:
    int update(const char* comPort);
    double getTemperature();
    double getPressure();
    double getHumidity();
    double getDewPoint();
};

