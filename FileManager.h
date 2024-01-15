#pragma once
#include "ArduinoJson.h"
#include "Sensor.h"

class FileManager
{
std::string historyPath = "history.json";
std::string configPath = "config.cfg";


public:
	int checkHistory(int newHistory);
	std::string loadHistory();
	int saveReadings(char readingsJSON[256], int newHistory);
	
	Sensor loadCalibration(Sensor sensor);
	void saveCalibration(std::string calibration);
};

