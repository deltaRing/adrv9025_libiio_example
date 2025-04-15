#ifndef UDPMODULE_H
#define UDPMODULE_H

#include <QDebug>
#include <string>
#include <complex>
#include <iostream>
#include <winsock2.h>
#define _ROWS_ 64
#define _COLS_ 8192

#pragma comment(lib, "Ws2_32.lib")

class UDPmodule
{
    public:
        UDPmodule();
        ~UDPmodule();
        // defined ip
        void setIP(std::string new_ip);
        void setPort(unsigned int new_port);
        void receiveData();
        std::complex<float> * getData() { if (ready) { ready = false; return data; } else return NULL; }

    private:
        WSADATA wsaData;
        SOCKET recvSocket;
        sockaddr_in serverAddr, clientAddr;
        int clientAddrLen = sizeof(clientAddr);
        unsigned char buffer[_ROWS_ * sizeof(std::complex<float>)];

        std::string ip = "192.168.1.4";
        unsigned int port = 8888;
        bool ready = false;
        std::complex<float> temp[_ROWS_ * _COLS_];
        std::complex<float> data[_ROWS_ * _COLS_];
        int _current_row_ = 0;
};

#endif // UDPMODULE_H
