#include "sensor.h"
#include "lib/serialib.h"
#include <chrono>
#include <iomanip>
#include <fstream>
#include <iostream>
#include <ArduinoJson.h>

int Sensor::update(const char* comPort) {
    serialib serial;
    char errorOpening = serial.openDevice(comPort, 9600);
    if (errorOpening != 1) {
        printf("Error connecting to %s %i\n",comPort, (int)errorOpening);
        return errorOpening;
    }
    printf("Successful connection to %s\n", comPort);

    char buffer[128] = "";
    Sensor sensor;

    while (strlen(buffer) < 10) {
        serial.readString(buffer, '\n', 128, 4000);
        printf("String read: %s\n", buffer);
        std::istringstream iss(buffer);
        while (iss) {
            std::string token;
            iss >> token;

            if (token == "Temperature") {
                iss >> token >> this->temperature >> token >> token;
            }
            if (token == "Pressure") {
                iss >> token >> this->pressure >> token >> token;
            }
            if (token == "Humidity") {
                iss >> token >> this->humidity >> token >> token;
            }
        }
        this->dewPoint = 
            (this->temperature - (14.55 + 0.114 * this->temperature) * (1 - (0.01 * this->humidity)) 
            - pow(((2.5 + 0.007 * this->temperature) * (1 - (0.01 * this->humidity))), 3) 
            - (15.9 + 0.117 * this->temperature) * pow((1 - (0.01 * this->humidity)), 14));
        std::time_t now = (std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()));
        localtime_s(&this->localTime, &now);
        std::stringstream ss;
        ss << std::put_time(&this->localTime , "%Y-%m-%dT%H:%M:%S");
        this->time = ss.str();
    }
    return 1;
}

double Sensor::getTemperature() {
    return ((this->temperature) + 273.15) * this->tempA + this->tempB - 273.15;
}

double Sensor::getPressure() {
    return this->pressure * this->presA + this->presB;
}

double Sensor::getHumidity() {
    return this->humidity * this->humiA + this->humiB;
}

double Sensor::getDewPoint() {
    return ((this->dewPoint) + 273.15) * this->dewpA + this->dewpB - 273.15;
}

std::string Sensor::getTime() {
    return this->time;
}

double Sensor::getTempA() {
    return this->tempA;
}

double Sensor::getTempB() {
    return this->tempB;
}

double Sensor::getHumiA() {
    return this->humiA;
}

double Sensor::getHumiB() {
    return this->humiB;
}

double Sensor::getPresA() {
    return this->presA;
}

double Sensor::getPresB() {
    return this->presB;
}

double Sensor::getDewpA() {
    return this->dewpA;
}

double Sensor::getDewpB() {
    return this->dewpB;
}

void Sensor::setCalibration(double tempA, double tempB, double humiA,
    double humiB, double presA, double presB, double dewpA, double dewpB) {
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
}

void Sensor::restoreCalibration() {
    this->tempA = 1;
    this->tempB = -4;
    this->humiA = 1;
    this->humiB = 0;
    this->presA = 1;
    this->presB = 0;
    this->dewpA = 1;
    this->dewpB = 0;
}

void Sensor::saveCalibration(std::string calibration) {
    std::string file_path = "config.cfg";
    std::ofstream file;

    file.open(file_path, std::ios::trunc);
    if (file.is_open()) {
        file << calibration; // Write string data to the file
        file.close(); // Close the file
        std::cout << "Data written to file successfully." << std::endl;
    }
    else {
        std::cerr << "Unable to open file: " << file_path << std::endl;
    }
}

