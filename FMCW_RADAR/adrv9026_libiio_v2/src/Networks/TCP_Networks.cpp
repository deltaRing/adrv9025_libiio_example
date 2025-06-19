#include "../include/Networks/TCP_Networks.h"

void send_tcp_data(struct sockaddr_in & serverAddr, int & sockfd, unsigned char * data, unsigned int datasize) {
  send(sockfd, data, datasize, 0);
}


void init_tcp_socket(struct sockaddr_in & server_addr, int & sockfd) {
  // 1. ?? socket
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    perror("socket creation failed");
    return;
  }

  // 2. ???????
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  inet_pton(AF_INET, SERVER_TCP_IP, &server_addr.sin_addr);

  // 3. ?????
  if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
    perror("connect failed");
    return;
  }
}

void close_tcp_socket(int & sockfd) {
  close(sockfd);
}