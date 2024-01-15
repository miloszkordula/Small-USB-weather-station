#include "Server.h"



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

    printf("Server created\n");
    this->mainSocket = mainSocket;
    fileManager.loadCalibration(sensor);
    fileManager.checkHistory(newHistory);

    return 0;
}

int Server::handleClient() {
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
        sensor.update();

        DynamicJsonDocument json(256);
        json["status"] = "ok";
        json["temperature"] = sensor.getTemperature();
        json["pressure"] = sensor.getPressure();
        json["dewPoint"] = sensor.getDewPoint();
        json["humidity"] = sensor.getHumidity();
        json["timeStamp"] = sensor.getTime();

        char jsonResponse[256] = "";
        serializeJson(json, jsonResponse);
        fileManager.saveReadings(jsonResponse, newHistory);
        if (sensor.getTime() != "") {
            std::string response = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: "
                + std::to_string(strlen(jsonResponse)) + "\r\n\r\n" + jsonResponse;
            send(clientSocket, response.c_str(), response.length(), 0);
        }
    }
    else if (request.find("GET /history") != std::string::npos) {
        std::string history;
        if (fileManager.loadHistory(history)) {
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
        json["tempA"] = sensor.getTempA();
        json["tempB"] = sensor.getTempB();
        json["humiA"] = sensor.getHumiA();
        json["humiB"] = sensor.getHumiB();
        json["presA"] = sensor.getPresA();
        json["presB"] = sensor.getPresB();
        json["dewpA"] = sensor.getDewpA();
        json["dewpB"] = sensor.getDewpB();
        json["comPort"] = sensor.getSerialPort();

        char jsonResponse[1024] = "";
        serializeJson(json, jsonResponse);
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

        sensor.setCalibration(doc["tempA"], doc["tempB"], doc["humiA"],
            doc["humiB"], doc["presA"], doc["presB"], doc["dewpA"], doc["dewpB"], doc["comPort"]);
        std::cout << "Calibration done\n";
        fileManager.saveCalibration(requestBody);

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


