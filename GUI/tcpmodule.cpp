#include "tcpmodule.h"

TCPmodule::TCPmodule()
{
    // 初始化 WinSock
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    // 创建 socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    // 设置地址结构
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY; // 任意本地 IP
    serverAddr.sin_port = htons(port);

    // 绑定 socket
    bind(serverSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));

    // 开始监听
    listen(serverSocket, SOMAXCONN);
    std::cout << "Server listening on port 8888..." << std::endl;

    // 接受客户端连接
    clientSocket = accept(serverSocket, (SOCKADDR*)&clientAddr, &clientAddrLen);
    std::cout << "Client connected." << std::endl;

}

void TCPmodule::setIP(std::string new_ip) {
    ip = new_ip;
}

void TCPmodule::setPort(unsigned int new_port) {
    port = new_port;
}

void TCPmodule::receiveData() {
    int len = recv(clientSocket, (char *) buffer, sizeof(buffer), 0);
    if (len <= 0) return;
    memcpy(data, buffer, sizeof(buffer));
    ready = true;
}
