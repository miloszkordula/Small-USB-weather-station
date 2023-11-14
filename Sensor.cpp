#include "sensor.h"
#include "lib/serialib.h"

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