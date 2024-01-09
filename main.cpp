//for serial ports above "COM9", we must use this extended syntax of "\\.\COMx".
#include <stdio.h>
#include "Server.h"

const char* serialPort = "\\\\.\\COM3";
int error = 0;

int main(){
 
    Server server;
    error = server.createServer();
    if (error == 1) return 1;
    while (true) {
        error = server.handleClient(serialPort);
        if (error == 1) return 1;
    }

    return 0;
}