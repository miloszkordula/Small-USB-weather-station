//for serial ports above "COM9", we must use this extended syntax of "\\.\COMx".
#include <stdio.h>
#include "Server.h"

const char* serialPort = "\\\\.\\COM4";
int main(){
 
    Server server;
    server.createServer();

    while (true) {
        server.handleClient(serialPort);
    }

    return 0;
}