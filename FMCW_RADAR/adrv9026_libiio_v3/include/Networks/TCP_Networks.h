#ifndef _TCP_NETWORKS_H_
#define _TCP_NETWORKS_H_

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8888
#define SERVER_TCP_IP "192.168.1.3" // ??? IP (target IP)
#define BUF_SIZE 1024 * 8 * 64

void send_tcp_data(struct sockaddr_in & serverAddr, int & sockfd, unsigned char * data, unsigned int datasize);
void init_tcp_socket(struct sockaddr_in & serverAddr, int & sockfd);
void close_tcp_socket(int & sockfd);

#endif