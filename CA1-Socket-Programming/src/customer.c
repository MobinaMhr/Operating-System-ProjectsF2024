#include "customer.h"

char *NAME;

void initCstmr(CustomerInfo *cstmr, unsigned short udpPort, const char *udpIp)
{
    cstmr->udpPort = udpPort;
    cstmr->udpFd = createUdpSckt(cstmr->udpPort, udpIp, &cstmr->udpAddr);
    cstmr->tcpPort = genUniqueTcpPort();
    cstmr->tcpFd = createTcpSckt(cstmr->tcpPort);
    cstmr->name = uniqueUsrName(cstmr->udpFd, cstmr->tcpFd, cstmr->udpAddr, cstmr->tcpPort);
    NAME = cstmr->name;
    logFile(NAME, "Initiated\n");
}

void displayMenu(char **menu)
{
    startDisply();
    for (int i = 0; menu[i] != NULL; i++)
    {
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "\n%d- %s", i + 1, menu[i]);
        setPromptReadInput(STDOUT_FILENO, buffer, NULL, 0);
    }
    endDisply();

    for (int i = 0; menu[i] != NULL; i++)
        free(menu[i]);
    free(menu);
    logFile(NAME, "Menu displayed\n");
}

OdrFood orderFood()
{
    OdrFood food;
    char buff[INP_BUFF_SIZE];
    memset(buff, '\0', INP_BUFF_SIZE);
    setPromptReadInput(STDOUT_FILENO, "\n-> name of food: ", buff, INP_BUFF_SIZE);
    strcpy(food.name, buff);
    memset(buff, '\0', INP_BUFF_SIZE);
    setPromptReadInput(STDOUT_FILENO, "-> port of restaurant: ", buff, INP_BUFF_SIZE);
    food.rstrntPort = atoi(buff);
    return food;
}

void showOrdrFoodRspnse(char *rspns, char *cstmrName, char *rstrntName)
{
    if (strcmp(rspns, YES) == 0)
    {
        logFile(NAME, "Restaurant accepted\n");
        printResponse(rstrntName, "Restaurant accepted and your food is ready!\n");
    }
    else if (strcmp(rspns, NO) == 0)
    {
        logFile(NAME, "Restaurant denied\n");
        printResponse(rstrntName, "Restaurant denied and cry about it!\n");
    }
    else
    {
        logFile(NAME, "Invalid income msg\n");
        setPromptReadInput(STDOUT_FILENO, "invalid income msg\n", NULL, 0);
    }
}

void sendOrdrFoodReq(char *sndrName, unsigned short int sndrPort)
{
    OdrFood food = orderFood();

    int fd = connectTcpClient(food.rstrntPort);
    if (fd < 0)
    {
        setPromptReadInput(STDERR_FILENO, "you entered wrong port.\n", NULL, 0);
        return;
    }

    char contnt[INP_BUFF_SIZE];
    memset(contnt, '\0', INP_BUFF_SIZE);
    sprintf(contnt, "%s|%d", food.name, fd);

    const char *enMsg = encodeMsg(creatMsg(sndrName, sndrPort, contnt, ORDER_FOOD));

    send(fd, enMsg, strlen(enMsg), 0);

    setPromptReadInput(STDOUT_FILENO, WAIT_TILL_RESTRNT_RESPONSE, NULL, 0);
    logFile(NAME, "Order food request sent\n");

    getOrdrFoodRspnse(sndrName, sndrPort, fd);
}

void getOrdrFoodRspnse(char *cstmrName, unsigned short cstmrTcpPort, int fd)
{
    signal(SIGALRM, alarmHndlr);
    siginterrupt(SIGALRM, 1);

    printf("|0|\n");
    printf("fd:%d\n", fd);
    alarm(90);
    consoleLock();
    char buff[INP_BUFF_SIZE];
    memset(buff, '\0', INP_BUFF_SIZE);
    int rcv = recv(fd, buff, INP_BUFF_SIZE, 0);
    alarm(0);
    consoleUnlock();
    printf("|1|\n");

    if (rcv <= 0)
    {
        setPromptReadInput(STDOUT_FILENO, TIMEOUT_MESSAGE, NULL, 0);
        char *encMsg = encodeMsg(creatMsg(cstmrName, cstmrTcpPort, ORDER_FOOD, TIMEOUT_MESSAGE));
        send(fd, encMsg, strlen(encMsg), 0);
    }

    printf("|2|\n");
    Message rspMsg = decodeMsg(buff);
    printf("|3|\n");
    showOrdrFoodRspnse(rspMsg.content, cstmrName, rspMsg.senderName);
}

void cmdLineInterface(CustomerInfo *cstmr, const char *cmd)
{
    logFile(NAME, "Command line interface started\n");
    if (cmd == NULL || strlen(cmd) == 0)
        return;
    else if (strcmp(cmd, SHOW_RESTAURANTS) == 0)
    {
        startDisply();
        setPromptReadInput(STDOUT_FILENO, SHOW_RESTAURANT_TITLE, NULL, 0);
        char buff[INP_BUFF_SIZE];
        char *msg = encodeMsg(creatMsg(cstmr->name, cstmr->tcpPort, SHOW_RESTAURANTS, SHOW_RESTAURANTS));
        int a = sendto(cstmr->udpFd, msg, strlen(msg), 0, (struct sockaddr *)&cstmr->udpAddr, sizeof(cstmr->udpAddr));

        signal(SIGALRM, alarmHndlr);
        siginterrupt(SIGALRM, 1);

        while (1)
        {
            alarm(1);
            int fd = accTcpClient(cstmr->tcpFd);
            memset(buff, '\0', INP_BUFF_SIZE);
            int rcv = recv(fd, buff, INP_BUFF_SIZE, 0);
            alarm(0);
            if (rcv < 0)
                break;
            Message msg = decodeMsg(buff);
            showUser(msg.senderName, msg.senderTcpPort);
        }

        endDisply();
    }
    else if (strcmp(cmd, SHOW_MENU) == 0)
    {
        displayMenu(getMenu());
    }
    else if (strcmp(cmd, ORDER_FOOD) == 0)
    {
        sendOrdrFoodReq(cstmr->name, cstmr->tcpPort);
    }
    else
    {
        setPromptReadInput(STDOUT_FILENO, "invalid command\n", NULL, 0);
    }
}

int tcpHndlr(Message *msg, CustomerInfo *cstmr)
{
    logFile(NAME, "TCP message received\n");
    if (strcmp(msg->title, SHOW_RESTAURANTS) == 0)
    {
        logFile(NAME, "SHOW_RESTAURANTS received\n");
        startDisply();
        setPromptReadInput(STDOUT_FILENO, SHOW_RESTAURANT_TITLE, NULL, 0);
        endDisply();
    }
    if (strcmp(msg->title, "lablab") == 0)
    {
        //
    }
    return 0;
}

int udbHndlr(Message *msg, CustomerInfo *cstmr)
{
    logFile(NAME, "UDP message received\n");
    char buff[INP_BUFF_SIZE];
    if ((strcmp(msg->title, CHECK_USERNAME_COMMAND) == 0) &&
        msg->senderTcpPort != cstmr->tcpPort)
    {
        logFile(NAME, "CHECK_USERNAME_COMMAND received\n");
        if (strcmp(cstmr->name, msg->senderName) != 0)
            return 0;
        Message bcMsg = creatMsg(cstmr->name, cstmr->tcpPort, CHECK_USERNAME_COMMAND, CHECK_USERNAME_COMMAND);

        int serverFd = connectTcpClient(msg->senderTcpPort);
        char *encodedMessage = encodeMsg(bcMsg);
        send(serverFd, encodedMessage, strlen(encodedMessage), 0);
        close(serverFd);
    }
    if (strcmp(msg->title, START_WORKING) == 0)
    {
        logFile(NAME, "START_WORKING received\n");
        memset(buff, '\0', INP_BUFF_SIZE);
        sprintf(buff, "%s %s\n", msg->senderName, RESTAURANT_OPPENED_MESSAGE);
        setPromptReadInput(STDOUT_FILENO, buff, NULL, 0);
    }
    if (strcmp(msg->title, BREAK) == 0)
    {
        logFile(NAME, "BREAK received\n");
        memset(buff, '\0', INP_BUFF_SIZE);
        sprintf(buff, "%s %s\n", msg->senderName, RESTAURANT_CLOSED_MESSAGE);
        setPromptReadInput(STDOUT_FILENO, buff, NULL, 0);
    }
    return 0;
}

int main(int argc, char const *argv[])
{
    int filFd = open("Customer", O_RDONLY);

    CustomerInfo cstmr;
    initCstmr(&cstmr, atoi(argv[1]), IP_ADDR);
    welcome(cstmr.name, Customer);

    fd_set masterSet, workingSet;
    struct sockaddr_in addrOut;

    int maxSd = (cstmr.tcpFd > cstmr.udpFd) ? cstmr.tcpFd : cstmr.udpFd;
    FD_ZERO(&masterSet);
    FD_SET(cstmr.udpFd, &masterSet);
    FD_SET(cstmr.tcpFd, &masterSet);
    FD_SET(STDIN_FILENO, &masterSet);
    char buffer[INP_BUFF_SIZE];
    while (1)
    {
        workingSet = masterSet;
        select(maxSd + 1, &workingSet, NULL, NULL, NULL);

        for (int i = 0; i <= maxSd; i++)
        {
            if (!FD_ISSET(i, &workingSet))
                continue;
            memset(buffer, '\0', INP_BUFF_SIZE);
            if (i == cstmr.udpFd)
            {
                recv(cstmr.udpFd, buffer, INP_BUFF_SIZE, 0);
                Message msg = decodeMsg(buffer);
                udbHndlr(&msg, &cstmr);
            }
            else if (i == cstmr.tcpFd)
            {
                int new_socket = accTcpClient(cstmr.tcpFd);
                FD_SET(new_socket, &masterSet);
                if (new_socket > maxSd)
                    maxSd = new_socket;
            }
            else if (i == STDIN_FILENO)
            {
                setPromptReadInput(STDIN_FILENO, NULL, buffer, INP_BUFF_SIZE);
                cmdLineInterface(&cstmr, buffer);
            }
            else
            {
                recv(i, buffer, INP_BUFF_SIZE, 0);
                Message msg = decodeMsg(buffer);
                tcpHndlr(&msg, &cstmr);
            }
        }
    }
    close(filFd);
}
