#include "sensor.h"
#include "lib/serialib.h"
#include <chrono>
#include <iomanip>
#include <fstream>
#include <iostream>

int Sensor::update(const char* comPort) {
    serialib serial;
    char errorOpening = serial.openDevice(comPort, 9600);
    if (errorOpening != 1) {
        printf("Error connecting to %s %i\n",comPort, (int)errorOpening);
        return errorOpening;
    }

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
        std::cout << "Calibration written to file successfully." << std::endl;
    }
    else {
        std::cerr << "Unable to open file: " << file_path << std::endl;
    }
}

Sensor::Sensor(){}

Sensor::Sensor(const Sensor& other) {
    temperature = other.temperature;
    pressure = other.pressure;
    humidity = other.humidity;
    dewPoint = other.dewPoint;
    time = other.time;
    localTime = other.localTime;
    tempA = other.tempA;
    tempB = other.tempB;
    humiA = other.humiA;
    humiB = other.humiB;
    presA = other.presA;
    presB = other.presB;
    dewpA = other.dewpA;
    dewpB = other.dewpB;
}

// Move constructor
Sensor::Sensor(Sensor&& other) noexcept {
    temperature = std::move(other.temperature);
    pressure = std::move(other.pressure);
    humidity = std::move(other.humidity);
    dewPoint = std::move(other.dewPoint);
    time = std::move(other.time);
    localTime = std::move(other.localTime);
    tempA = std::move(other.tempA);
    tempB = std::move(other.tempB);
    humiA = std::move(other.humiA);
    humiB = std::move(other.humiB);
    presA = std::move(other.presA);
    presB = std::move(other.presB);
    dewpA = std::move(other.dewpA);
    dewpB = std::move(other.dewpB);
}

// Copy assignment operator
 Sensor& Sensor::operator=(const Sensor& other) {
    if (this != &other) {
        temperature = other.temperature;
        pressure = other.pressure;
        humidity = other.humidity;
        dewPoint = other.dewPoint;
        time = other.time;
        localTime = other.localTime;
        tempA = other.tempA;
        tempB = other.tempB;
        humiA = other.humiA;
        humiB = other.humiB;
        presA = other.presA;
        presB = other.presB;
        dewpA = other.dewpA;
        dewpB = other.dewpB;
    }
    return *this;
}

// Move assignment operator
Sensor& Sensor::operator=(Sensor&& other) noexcept {
    if (this != &other) {
        temperature = std::move(other.temperature);
        pressure = std::move(other.pressure);
        humidity = std::move(other.humidity);
        dewPoint = std::move(other.dewPoint);
        time = std::move(other.time);
        localTime = std::move(other.localTime);
        tempA = std::move(other.tempA);
        tempB = std::move(other.tempB);
        humiA = std::move(other.humiA);
        humiB = std::move(other.humiB);
        presA = std::move(other.presA);
        presB = std::move(other.presB);
        dewpA = std::move(other.dewpA);
        dewpB = std::move(other.dewpB);
    }
    return *this;
}
