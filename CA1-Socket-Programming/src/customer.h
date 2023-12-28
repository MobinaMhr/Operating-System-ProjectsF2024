#ifndef CUSTOMER_H
#define CUSTOMER_H

#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/socket.h>

#include "network.h"
#include "defines.h"
// #include "logger.h"
#include "utils.h"
#include "JSONParser.h"
#include "structures.h"

void initCstmr(CustomerInfo *cstmr, unsigned short udpPort, const char *udpIp);

void displayMenu(char **menu);

OdrFood orderFood();

void showOrdrFoodRspnse(char *rspns, char *cstmrName, char *rstrntName);

void sendOrdrFoodReq(char *sndrName, unsigned short int sndrPort);

void getOrdrFoodRspnse(char *cstmrName, unsigned short cstmrTcpPort, int fd);

void cmdLineInterface(CustomerInfo *cstmr, const char *cmd);

int tcpHndlr(Message *msg, CustomerInfo *cstmr);

int udbHndlr(Message *msg, CustomerInfo *cstmr);

#endif

