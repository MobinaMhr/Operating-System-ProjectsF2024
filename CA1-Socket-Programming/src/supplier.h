#ifndef SUPPLIER_H
#define SUPPLIER_H

#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <unistd.h>

#include "network.h"
#include "defines.h"
#include "utils.h"
#include "JSONParser.h"
// #include "logger.h"

void initSpplr(SupplierInfo *spplr, unsigned short udpPort, const char *udpIp);

void initStore(SupplierInfo *spplr);

void cmdLineInterface(SupplierInfo *spplr, const char *cmd);

int udbHndlr(Message *msg, SupplierInfo *spplr);

int tcpHndlr(SupplierInfo *spplr, Message *msg);

#endif