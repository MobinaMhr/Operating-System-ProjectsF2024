#ifndef NETWORK_H
#define NETWORK_H

#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <asm-generic/socket.h>

#include "utils.h"

int createUdpSckt(const int port, const char *udpIp, struct sockaddr_in *addr);

int createTcpSckt(int port);

int accTcpClient(int server_fd);

int connectTcpClient(int port);

#endif