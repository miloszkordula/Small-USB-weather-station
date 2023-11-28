//for serial ports above "COM9", we must use this extended syntax of "\\.\COMx".
#include <stdio.h>
#include <thread>
#include <atomic>
#include "Server.h"

#include <iostream>

const char* serialPort = "\\\\.\\COM3";

//std::atomic<int> isClientConnected(0);
Sensor sens;
std::atomic<Sensor> sensor;
std::atomic<int> error(0);
void thread(Server server, const char* serialPort) {
    while (true) {
       // Server serv2;
        //serv2 = server.load(std::memory_order_acquire);

        int err = server.handleClient(serialPort, sensor.load(std::memory_order_acquire));
        //serv2 = server.load(std::memory_order_acquire);
        error.store(err, std::memory_order_release);

    }
}


int main() {
    Server server;
    int err = server.createServer();
    if (err == 1) return 1;
   // server.store(serv, std::memory_order_release);
    std::thread handleClientThread(thread, server, serialPort);
    while (true) {
        if (error.load(std::memory_order_acquire) == 1) return 1;
           // if (!isClientConnected.load(std::memory_order_acquire)) {
               // serv = server.load(std::memory_order_acquire);
               sensor.store(server.noClient(serialPort),std::memory_order_release);
                //server.store(serv, std::memory_order_release);
                std::cout << "noclient \n";
           //}
          //  else continue;
        //}
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    return 0;
}