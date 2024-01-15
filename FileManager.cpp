#include "FileManager.h"


void FileManager::checkHistory(int newHistory) {
    std::fstream file(historyPath, std::ios::in | std::ios::out | std::ios::ate);

    if (!file.is_open()) {
        file.open(historyPath, std::ios::out);
        file << "[\n";
        file.close();
        newHistory = 1;
        std::cout << "No history file, creating new.\n";
    }
    else {
        file.seekg(0, std::ios::end);
        if (file.tellg() == 0) {
            file.close();
            file.open(historyPath, std::ios::out | std::ios::app);
            file << "[\n";
            file.close();
            newHistory = 1;
            std::cout << "History file exists, but is empty.\n";
        }
        else {
            file.close();
            std::cout << "History file already exists.\n";
        }
    }
}

int FileManager::loadHistory(std::string history) {
    std::ifstream file(historyPath);

    if (!file.is_open()) {
        std::cout << "Error opening the file!" << std::endl;
        return 0;
    }
    else {
        std::string line;
        while (std::getline(file, line)) {
            history += line + "\n";
        }
        file.close();
        history += " \n]";
        return 1;
    }
}

int FileManager::saveReadings(char readingsJSON[256], int newHistory) {
    std::ofstream file;

    file.open(historyPath, std::ios::app);
    if (!file.is_open()) {
        std::cout << "Error opening the file!" << std::endl;
        return 1;
    }
    if (newHistory == 0) {
        file << ',' << std::endl << readingsJSON;
        file.close();
    }
    else {
        file << std::endl << readingsJSON;
        file.close();
        newHistory = 0;
    }
    return 0;
}

void FileManager::loadCalibration(Sensor sensor) {
    std::ifstream file(configPath);
    std::string fileContent, line;

    if (file.is_open()) {
        while (std::getline(file, line)) {
            fileContent += line;
            fileContent += '\n';
        }
        file.close();
    }
    else {
        std::cerr << "Unable to open file: " << configPath << std::endl;
    }
    StaticJsonDocument<1024> doc;
    DeserializationError error = deserializeJson(doc, fileContent);

    if (error) {
        std::cout << "Error recieving values\n";
    }
    else {
        sensor.setCalibration(doc["tempA"], doc["tempB"], doc["humiA"],
            doc["humiB"], doc["presA"], doc["presB"], doc["dewpA"], doc["dewpB"], doc["comPort"]);
        std::cout << "Calibration loaded\n";
    }
}

void FileManager::saveCalibration(std::string calibration) {
    std::ofstream file;

    file.open(configPath, std::ios::trunc);
    if (file.is_open()) {
        file << calibration;
        file.close();
        std::cout << "Calibration written to file successfully." << std::endl;
    }
    else {
        std::cerr << "Unable to open file: " << configPath << std::endl;
    }
}