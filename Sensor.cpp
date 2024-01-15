#include "sensor.h"
#include "lib/serialib.h"
#include <chrono>
#include <iomanip>
#include <cstring>

int Sensor::update() {
    serialib serial;

    char errorOpening = serial.openDevice(this->serialPort, 9600);
    if (errorOpening != 1) {
        printf("Error connecting to %s %i\n",this->serialPort, (int)errorOpening);
        return errorOpening;
    }

    char buffer[128] = "";
    Sensor sensor;

    while (strlen(buffer) < 10) {
        std::time_t now = (std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()));
        localtime_s(&this->localTime, &now);
        std::stringstream ss;
        ss << std::put_time(&this->localTime, "%Y-%m-%dT%H:%M:%S");
        serial.readString(buffer, '\n', 128, 4000);
        std::cout << ss.str();
        printf(" Data read: %s", buffer);
        std::istringstream iss(buffer);
        while (iss) {
            std::string token;
            iss >> token;

            if (token == "Temperature") {
                iss >> token >> temperature >> token >> token;
            }
            if (token == "Pressure") {
                iss >> token >> pressure >> token >> token;
            }
            if (token == "Humidity") {
                iss >> token >> humidity >> token >> token;
            }
        }
        dewPoint = 
            (temperature - (14.55 + 0.114 * temperature) * (1 - (0.01 * humidity)) 
            - pow(((2.5 + 0.007 * temperature) * (1 - (0.01 * humidity))), 3) 
            - (15.9 + 0.117 * temperature) * pow((1 - (0.01 * humidity)), 14));
      
        time = ss.str();
    }
    return 1;
}

double Sensor::getTemperature() {
    return ((temperature) + 273.15) * tempA + tempB - 273.15;
}

double Sensor::getPressure() {
    return pressure * presA + presB;
}

double Sensor::getHumidity() {
    return humidity * humiA + humiB;
}

double Sensor::getDewPoint() {
    return ((dewPoint) + 273.15) * dewpA + dewpB - 273.15;
}

std::string Sensor::getTime() {
    return time;
}

double Sensor::getTempA() {
    return tempA;
}

double Sensor::getTempB() {
    return tempB;
}

double Sensor::getHumiA() {
    return humiA;
}

double Sensor::getHumiB() {
    return humiB;
}

double Sensor::getPresA() {
    return presA;
}

double Sensor::getPresB() {
    return presB;
}

double Sensor::getDewpA() {
    return dewpA;
}

double Sensor::getDewpB() {
    return dewpB;
}

const char* Sensor::getSerialPort() {
    return serialPort;
}

void Sensor::setCalibration(double tempA, double tempB, double humiA,
    double humiB, double presA, double presB, double dewpA,
    double dewpB, const char* comPort) {
    if (tempA <= 0) tempA = 1;
    if (humiA <= 0) humiA = 1;
    if (presA <= 0) presA = 1;
    if (dewpA <= 0) dewpA = 1;
    this->tempA = tempA;
    this->tempB = tempB;
    this->humiA = humiA;
    this->humiB = humiB;
    this->presA = presA;
    this->dewpB = presB;
    this->dewpA = dewpA;
    this->dewpB = dewpB;
    this->serialPort = _strdup(comPort);
}

void Sensor::restoreCalibration() {
    tempA = 1;
    tempB = -4;
    humiA = 1;
    humiB = 0;
    presA = 1;
    presB = 0;
    dewpA = 1;
    dewpB = 0;
    serialPort = "\\\\.\\COM4";
}



