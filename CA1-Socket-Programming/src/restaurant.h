#ifndef RESTAURANT_H
#define RESTAURANT_H

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/types.h>

#include "defines.h"
#include "utils.h"
#include "JSONParser.h"
#include "network.h"

void initRstrnt(RestaurantInfo *rstrnt, unsigned short udpPort, const char *udpIp);

void initKitchen(RestaurantInfo *rtnt);

void display(const OrderedFood *orderedFood);

void displayRequestsList(RestaurantInfo *rstrnt);

void desplaySalesHistory(RestaurantInfo *rstrnt);

OrderedIngredient orderIngrdnt();

void displayIngredients(RestaurantInfo *rstrnt);

void displayFoodRecepies(Recipes recps);

void sendIngrdntReq(RestaurantInfo *rstrnt);

void showSuppliers(int rstrntTcpFd, char *rstrntName);

char *getIngrdntRspns(RestaurantInfo *rstrnt, int createdFd);

void makeFood(RestaurantInfo *rst, char *foodName);

void cmdLineInterface(RestaurantInfo *rstrnt, const char *cmd);

int isInKitchn(RestaurantInfo *rstrnt, char *ingName);

char *showOrdrIngrdntRspnse(char *rspns, char *rstrntName, char *spplrName);

int udbHndlr(Message *msg, RestaurantInfo *rstrnt);

Response answerOrderReq();

void tcpHndlr(RestaurantInfo *rstrnt, Message *msg);

#endif