#pragma once
#include "ArduinoJson.h"
#include "Sensor.h"

class FileManager
{
std::string historyPath = "history.json";
std::string configPath = "config.cfg";


public:
	void checkHistory(int newHistory);
	int loadHistory(std::string history);
	int saveReadings(char readingsJSON[256], int newHistory);
	
	void loadCalibration(Sensor sensor);
	void saveCalibration(std::string calibration);
};

