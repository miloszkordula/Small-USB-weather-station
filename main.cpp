//for serial ports above "COM9", we must use this extended syntax of "\\.\COMx".
#include <stdio.h>
#include "Server.h"

int error = 0;

int main(){
 
    Server server;
    error = server.createServer();
    if (error == 1) return 1;
    while (true) {
        error = server.handleClient();
        if (error == 1) return 1;
    }

    return 0;
}