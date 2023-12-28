#ifndef UTILS_H
#define UTILS_H

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>

#include "network.h"
#include "defines.h"
#include "structures.h"

void setPromptReadInput(int fd, const char *prmpt, char *dst, int dstLen);

void printResponse(const char *rstrntName, const char *response);

int genUniqueTcpPort();

void welcome(char *name, Client type);

int find_unused_port();

void alarmHndlr(int sig);

Message creatMsg(char *name, unsigned short int tcpPort, char *cntnt, char *title);

char *uniqueUsrName(int udpFd, int tcpFd, struct sockaddr_in udpAddr, unsigned short tcpPort);
// char *uniqueUsrName(unsigned short tcpPort, struct sockaddr_in addr, int udpFd, int tcpFd);

void startDisply();

void endDisply();

void showUser(char *name, unsigned short int port);

char *encodeMsg(Message msg);

Message decodeMsg(char *msg);

char *genWelcomeMsg(char *username, Client type);

int logFile(const char* name, const char* txt);

void consoleLock();

void consoleUnlock();

#endif