#include "../include/Networks/UDP_Networks.h"

void send_data(struct sockaddr_in & serverAddr, int & sockfd, unsigned char * data, unsigned int datasize){
  ssize_t sentLen = sendto(sockfd, data, datasize, 0, 
                                 (struct sockaddr*)&serverAddr, sizeof(serverAddr));
  if (sentLen < 0) {
    perror("UDP: Send Data Failed!\n");
  }
}

void init_socket(struct sockaddr_in & serverAddr, int & sockfd){
  // ?? UDP ???
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  // ???????
  memset(&serverAddr, 0, sizeof(serverAddr));
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(PORT);
  serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP); // ?? IP
}

void close_socket(int & sockfd) {
  close(sockfd);
}