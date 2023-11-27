//for serial ports above "COM9", we must use this extended syntax of "\\.\COMx".
#include <stdio.h>
#include <thread>
#include <atomic>
#include "Server.h"

#include <iostream>

const char* serialPort = "\\\\.\\COM3";

//std::atomic<int> isClientConnected(0);
Server serv;
std::atomic<Server> server(serv);
std::atomic<int> error(0);
void thread(const char* serialPort) {
    while (true) {
        Server serv2;
        serv2 = server.load(std::memory_order_acquire);
        int err = serv2.handleClient(serialPort);
        serv2 = server.load(std::memory_order_acquire);
        error.store(err, std::memory_order_release);

    }
}


int main() {
    Server serv;
    int err = serv.createServer();
    if (err == 1) return 1;
    server.store(serv, std::memory_order_release);
    std::thread handleClientThread(thread, serialPort);
    while (true) {
        if (error.load(std::memory_order_acquire) == 1) return 1;
           // if (!isClientConnected.load(std::memory_order_acquire)) {
                serv = server.load(std::memory_order_acquire);
                serv.noClient(serialPort);
                server.store(serv, std::memory_order_release);
                std::cout << "noclient \n";
           //}
          //  else continue;
        //}
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    return 0;
}