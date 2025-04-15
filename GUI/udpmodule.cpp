#include "udpmodule.h"

UDPmodule::UDPmodule()
{
    // 初始化 Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed.\n";
        return;
    }

    // 创建 Socket
    recvSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (recvSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed.\n";
        WSACleanup();
        return;
    }

    // 填写地址信息
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port); // 监听端口
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // 绑定
    if (bind(recvSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
         std::cerr << "Bind failed.\n";
         closesocket(recvSocket);
         WSACleanup();
    }
}

UDPmodule::~UDPmodule() {
    closesocket(recvSocket);
    WSACleanup();
}

void UDPmodule::setIP(std::string new_ip) {
    ip = new_ip;
}

void UDPmodule::setPort(unsigned int new_port){
    port = new_port;
}

void UDPmodule::receiveData(){
    for (int ii = 0; ii < _COLS_; ii++) {
        int bytesReceived = recvfrom(recvSocket, (char *)buffer, sizeof(buffer), 0,
                                            (SOCKADDR*)&clientAddr, &clientAddrLen);
        if (bytesReceived == SOCKET_ERROR) {
              std::cerr << "recvfrom() failed.\n";
        }

        int index = -1;
        if (bytesReceived == 4){
            memcpy(&index, buffer, sizeof(index));
        }else {
            continue;
        }

        bytesReceived = recvfrom(recvSocket, (char *)buffer, sizeof(buffer), 0,
                                            (SOCKADDR*)&clientAddr, &clientAddrLen);
        if (bytesReceived == SOCKET_ERROR) {
              std::cerr << "recvfrom() failed.\n";
        }
        // check

        for(int ii = 0; ii < _ROWS_; ii++) {
            memcpy(&temp[ii * _COLS_ + index], &buffer[ii * sizeof(std::complex<float>)], sizeof(std::complex<float>));
            // qDebug() << temp[ii * _COLS_ + index].real();
        }

        // memcpy((unsigned char *)temp + index * _COLS_ * sizeof (std::complex<float>), buffer, _COLS_ * sizeof (std::complex<float>));
        if (index == _COLS_ - 1) {
            ready = true;
            memcpy(data, temp, _ROWS_ * _COLS_ * sizeof (std::complex<float>));
        }
    }
}
