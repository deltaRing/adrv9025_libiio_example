#ifndef _UDP_NETWORKS_H_
#define _UDP_NETWORKS_H_

#include <iostream>
#include <cstring>      // memset
#include <sys/socket.h> // socket API
#include <netinet/in.h> // sockaddr_in
#include <arpa/inet.h>  // inet_addr
#include <unistd.h>     // close

#define SERVER_IP "192.168.1.3" // ??? IP (target IP)
#define PORT 8888

void send_data(struct sockaddr_in & serverAddr, int & sockfd, unsigned char * data, unsigned int datasize);
void init_socket(struct sockaddr_in & serverAddr, int & sockfd);
void close_socket(int & sockfd);

#endif