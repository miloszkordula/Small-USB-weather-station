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
    if (mainSocket == INVALID_SOCKET)
    {
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
       // continue;
    }

    std::string request(buffer, bytesRead);

    if (request.find("GET /index.html") != std::string::npos) {
        std::ifstream file("./html/index.html");
        if (file.is_open()) {
            std::string content((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));
            std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: " + std::to_string(content.length()) + "\r\n\r\n" + content;
            send(clientSocket, response.c_str(), response.length(), 0);
        }
        else {
            std::string response = "HTTP/1.1 404 Not Found\r\n\r\n<html><body><h1>404 Not Found</h1></body></html>";
            send(clientSocket, response.c_str(), response.length(), 0);
        }
    }
    else if (request.find("GET /style.css") != std::string::npos) {
        std::ifstream file("./html/style.css");
        printf("style\n");
        if (file.is_open()) {
            std::string content((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));
            std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/css\r\nContent-Length: " + std::to_string(content.length()) + "\r\n\r\n" + content;
            send(clientSocket, response.c_str(), response.length(), 0);
        }
        else {
            std::string response = "HTTP/1.1 404 Not Found\r\n\r\n<html><body><h1>404 Not Found</h1></body></html>";
            send(clientSocket, response.c_str(), response.length(), 0);
        }
    }
    else if (request.find("GET /main.js") != std::string::npos) {
        std::ifstream file("./html/main.js");
        printf("main\n");
        if (file.is_open()) {
            std::string content((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));
            std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/javascript\r\nContent-Length: " + std::to_string(content.length()) + "\r\n\r\n" + content;
            send(clientSocket, response.c_str(), response.length(), 0);
        }
        else {
            std::string response = "HTTP/1.1 404 Not Found\r\n\r\n<html><body><h1>404 Not Found</h1></body></html>";
            send(clientSocket, response.c_str(), response.length(), 0);
        }
    }
    else if (request.find("GET /sensorReadings") != std::string::npos) {
        this->sensor.update(comPort);

        DynamicJsonDocument json(1024);
        json["status"] = "ok";
        json["temperature"] = sensor.getTemperature();
        json["pressure"] = sensor.getPressure();
        json["altitude"] = 250.0F;
        json["humidity"] = sensor.getHumidity();

        // Serialize the JSON document to a string
        char jsonResponse[256] = "";
        serializeJson(json, jsonResponse);
        std::string response = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: " + std::to_string(strlen(jsonResponse)) + "\r\n\r\n" + jsonResponse;
        send(clientSocket, response.c_str(), response.length(), 0);
    }
    else {
        std::string response = "HTTP/1.1 404 Not Found\r\n\r\n<html><body><h1>404 Not Found</h1></body></html>";
        send(clientSocket, response.c_str(), response.length(), 0);
    }
    closesocket(clientSocket);
}
