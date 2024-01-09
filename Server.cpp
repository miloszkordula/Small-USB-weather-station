#include "Server.h"
#include "ArduinoJson.h"
#include <iostream>
#include <fstream>

int Server::createServer() {
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(80);
    
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != NO_ERROR)
        printf("Initialization error.\n");

    SOCKET mainSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (mainSocket == INVALID_SOCKET)  {
        printf("Error creating socket: %ld\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }


    if (bind(mainSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
        std::cerr << "Bind failed." << std::endl;
        closesocket(mainSocket);
        WSACleanup();
        return 1;
    }


    if (listen(mainSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Listen failed." << std::endl;
        closesocket(mainSocket);
        WSACleanup();
        return 1;
    }
    printf("server created\n");
    this->mainSocket = mainSocket;
    loadCalibration();
    std::string filename = "history.json";
    std::fstream file(filename, std::ios::in | std::ios::out | std::ios::ate);

    if (!file.is_open()) {
        file.open(filename, std::ios::out);
        file << "[\n";
        file.close();
        newHistory = 1;
    }
    else {
        file.seekg(0, std::ios::end);
        if (file.tellg() == 0) {
            file.close();
            file.open(filename, std::ios::out | std::ios::app);
            file << "[\n";
            file.close();
            newHistory = 1;
        }
        else {
            file.close();
            std::cout << "History already exists and is not empty.\n";
        }
    }

    return 0;
}

int Server::handleClient(const char* comPort) {
    SOCKET clientSocket = accept(mainSocket, NULL, NULL);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Accept failed." << std::endl;
        closesocket(mainSocket);
        WSACleanup();
        return 1;
    }

    char buffer[1024];
    int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesRead == SOCKET_ERROR) {
        std::cerr << "Receive failed." << std::endl;
        closesocket(clientSocket);
        return 0;
    }

    std::string request(buffer, bytesRead);

    if (request.find("GET /index.html") != std::string::npos) {
        std::ifstream file("./html/index.html");
        if (file.is_open()) {
            std::string content((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));
            std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: " 
                + std::to_string(content.length()) + "\r\n\r\n" + content;
            send(clientSocket, response.c_str(), response.length(), 0);
        }
        else {
            std::string response = "HTTP/1.1 404 Not Found\r\n\r\n<html><body><h1>404 Not Found</h1></body></html>";
            send(clientSocket, response.c_str(), response.length(), 0);
        }
    }
    else if (request.find("GET /style.css") != std::string::npos) {
        std::ifstream file("./html/style.css");
        if (file.is_open()) {
            std::string content((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));
            std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/css\r\nContent-Length: " 
                + std::to_string(content.length()) + "\r\n\r\n" + content;
            send(clientSocket, response.c_str(), response.length(), 0);
        }
        else {
            std::string response = "HTTP/1.1 404 Not Found\r\n\r\n<html><body><h1>404 Not Found</h1></body></html>";
            send(clientSocket, response.c_str(), response.length(), 0);
        }
    }
    else if (request.find("GET /main.js") != std::string::npos) {
        std::ifstream file("./html/main.js");
        if (file.is_open()) {
            std::string content((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));
            std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/javascript\r\nContent-Length: " 
                + std::to_string(content.length()) + "\r\n\r\n" + content;
            send(clientSocket, response.c_str(), response.length(), 0);
        }
        else {
            std::string response = "HTTP/1.1 404 Not Found\r\n\r\n<html><body><h1>404 Not Found</h1></body></html>";
            send(clientSocket, response.c_str(), response.length(), 0);
        }
    }
    else if (request.find("GET /sensorReadings") != std::string::npos) {
        this->sensor.update(comPort);

        DynamicJsonDocument json(256);
        json["status"] = "ok";
        json["temperature"] = sensor.getTemperature();
        json["pressure"] = sensor.getPressure();
        json["dewPoint"] = sensor.getDewPoint();
        json["humidity"] = sensor.getHumidity();
        json["timeStamp"] = sensor.getTime();

        char jsonResponse[256] = "";
        serializeJson(json, jsonResponse);
        saveReadingsToFile(jsonResponse);
        if (sensor.getTime() != "") {
            std::string response = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: "
                + std::to_string(strlen(jsonResponse)) + "\r\n\r\n" + jsonResponse;
            send(clientSocket, response.c_str(), response.length(), 0);
        }
    }
    else if (request.find("GET /history") != std::string::npos) {
        std::string file_path = "history.json";
        std::string history;
        std::ifstream file(file_path);

        if (!file.is_open()) {
            std::cout << "Error opening the file!" << std::endl;
        }
        else {
            std::string line;
            while (std::getline(file, line)) {
                history += line + "\n";
            }
            file.close();
            history += " \n]";

            std::string response = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: "
                + std::to_string(history.length()) + "\r\n\r\n" + history;
            send(clientSocket, response.c_str(), response.length(), 0);
        }
    }
    else if (request.find("GET /calibration.html") != std::string::npos) {
        std::ifstream file("./html/calibration.html");
        if (file.is_open()) {
            std::string content((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));
            std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: "
                + std::to_string(content.length()) + "\r\n\r\n" + content;
            send(clientSocket, response.c_str(), response.length(), 0);
        }
        else {
            std::string response = "HTTP/1.1 404 Not Found\r\n\r\n<html><body><h1>404 Not Found</h1></body></html>";
            send(clientSocket, response.c_str(), response.length(), 0);
        }
    }
    else if (request.find("GET /calibration.js") != std::string::npos) {
        std::ifstream file("./html/calibration.js");
        if (file.is_open()) {
            std::string content((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));
            std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/javascript\r\nContent-Length: "
                + std::to_string(content.length()) + "\r\n\r\n" + content;
            send(clientSocket, response.c_str(), response.length(), 0);
        }
        else {
            std::string response = "HTTP/1.1 404 Not Found\r\n\r\n<html><body><h1>404 Not Found</h1></body></html>";
            send(clientSocket, response.c_str(), response.length(), 0);
        }
    }
    else if (request.find("GET /calibrationValues") != std::string::npos) {

        DynamicJsonDocument json(1024);
        json["tempA"] = this->sensor.getTempA();
        json["tempB"] = this->sensor.getTempB();
        json["humiA"] = this->sensor.getHumiA();
        json["humiB"] = this->sensor.getHumiB();
        json["presA"] = this->sensor.getPresA();
        json["presB"] = this->sensor.getPresB();
        json["dewpA"] = this->sensor.getDewpA();
        json["dewpB"] = this->sensor.getDewpB();

        char jsonResponse[1024] = "";
        serializeJson(json, jsonResponse);
        //saveReadingsToFile(jsonResponse);
        std::string response = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: "
            + std::to_string(strlen(jsonResponse)) + "\r\n\r\n" + jsonResponse;
        send(clientSocket, response.c_str(), response.length(), 0);
        }
    else if (request.find("POST /submitValues") != std::string::npos) {
        std::string requestBody = request.substr(request.find("\r\n\r\n") + 4);
        StaticJsonDocument<1024> doc;
        DeserializationError error = deserializeJson(doc, requestBody);

        if (error) {
            std::cout << "Error recieving values\n";
            return 0;
        }

        this->sensor.setCalibration(doc["tempA"], doc["tempB"], doc["humiA"],
            doc["humiB"], doc["presA"], doc["presB"], doc["dewpA"], doc["dewpB"] );
        std::cout << "Calibration done\n";
        this->sensor.saveCalibration(requestBody);

        std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<html><body><h1>Received inputs successfully</h1></body></html>";
        send(clientSocket, response.c_str(), response.length(), 0);
    }
    else {
        std::string response = "HTTP/1.1 404 Not Found\r\n\r\n<html><body><h1>404 Not Found</h1></body></html>";
        send(clientSocket, response.c_str(), response.length(), 0);
    }
    closesocket(clientSocket);
    return 0;
}

int Server::saveReadingsToFile(char readingsJSON[256]) {
    std::string file_path = "history.json";
    std::ofstream file;

    file.open(file_path, std::ios::app);
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
    std::cout << "Readings saved" << std::endl;
    return 0;
}

void Server::loadCalibration() {
    std::string file_path = "config.cfg";
    std::ifstream file(file_path);
    std::string fileContent, line;

    if (file.is_open()) {
        while (std::getline(file, line)) {
            fileContent += line;
            fileContent += '\n';
        }
        file.close();
    }
    else {
        std::cerr << "Unable to open file: " << file_path << std::endl;
    }
    StaticJsonDocument<1024> doc;
    DeserializationError error = deserializeJson(doc, fileContent);

    if (error) {
        std::cout << "Error recieving values\n";
    }
    else {
       this->sensor.setCalibration(doc["tempA"], doc["tempB"], doc["humiA"],
            doc["humiB"], doc["presA"], doc["presB"], doc["dewpA"], doc["dewpB"]);
       std::cout << "Calibration loaded\n";
    }
}