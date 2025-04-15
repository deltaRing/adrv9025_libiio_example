#ifndef TCPMODULE_H
#define TCPMODULE_H


#include "udpmodule.h"
#include <winsock2.h>
#include <iostream>
#pragma comment(lib, "ws2_32.lib")

class TCPmodule
{
public:
    TCPmodule();
    void setIP(std::string new_ip);
    void setPort(unsigned int new_port);
    void receiveData();
    std::complex<float> * getData() { if (ready) { ready = false; return data; } else return NULL; }

private:
    WSADATA wsaData;
    SOCKET serverSocket, clientSocket;
    sockaddr_in serverAddr, clientAddr;
    int clientAddrLen = sizeof(clientAddr);
    unsigned char buffer[_ROWS_ * _COLS_ * sizeof(std::complex<float>)];

    std::string ip = "192.168.1.4";
    unsigned int port = 8888;
    bool ready = false;
    std::complex<float> temp[_ROWS_ * _COLS_];
    std::complex<float> data[_ROWS_ * _COLS_];
    int _current_row_ = 0;
};

#endif // TCPMODULE_H
