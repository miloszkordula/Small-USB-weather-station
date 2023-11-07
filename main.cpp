#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment (lib, "ws2_32.lib")
//for serial ports above "COM9", we must use this extended syntax of "\\.\COMx".
#define SERIAL_PORT "\\\\.\\COM3"

#include <WinSock2.h>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include "lib/serialib.h"
#include "ArduinoJson.h"


int main(){
    float temperature = 888.8f;
    float pressure = 888.8f;
    float humidity = 888.8f;

    WSADATA wsaData;

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

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(80);

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

    while (true) {
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
            continue;
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

            serialib serial;
            char errorOpening = serial.openDevice(SERIAL_PORT, 9600);
            if (errorOpening != 1) return errorOpening;
            printf("Successful connection to %s\n", SERIAL_PORT);

            char buffer[128] = "";

            while (strlen(buffer) <10) {
                serial.readString(buffer, '\n', 128, 4000);
                printf("String read: %s\n", buffer);
                

                // Create an input string stream from the input string
                std::istringstream iss(buffer);

                // Use a loop to extract values
                while (iss) {
                    std::string token;
                    iss >> token;

                    if (token == "Temperature") {
                        iss >> temperature;
                    }
                    else if (token == "Pressure") {
                        iss >> pressure;
                    }
                    else if (token == "Humidity") {
                        iss >> humidity;
                    }
                }
            }
           // serial.closeDevice();

            DynamicJsonDocument json(1024);
            json["status"] = "ok";
            json["temperature"] = temperature;
            json["pressure"] = pressure / 100.0F;
            json["altitude"] = 250.0F;
            json["humidity"] = humidity;

            // Serialize the JSON document to a string
            char jsonResponse[256] = "";
            serializeJson(json, jsonResponse);
            std::string response = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: " + std::to_string(strlen(jsonResponse)) + "\r\n\r\n" + jsonResponse;
            send(clientSocket, response.c_str(), response.length(), 0);

            //serial.closeDevice();
        }
        else {
            std::string response = "HTTP/1.1 404 Not Found\r\n\r\n<html><body><h1>404 Not Found</h1></body></html>";
            send(clientSocket, response.c_str(), response.length(), 0);
        }




        closesocket(clientSocket);
    }



    return 0;
}