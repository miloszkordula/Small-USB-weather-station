#include "sensor.h"
#include "lib/serialib.h"
#include <chrono>
#include <iomanip>

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
    return this->temperature;
}

double Sensor::getPressure() {
    return this->pressure;
}

double Sensor::getHumidity() {
    return this->humidity;
}

double Sensor::getDewPoint() {
    return this->dewPoint;
}

std::string Sensor::getTime() {
    return this->time;
}