#include "supplier.h"

char *NAME;

void initSpplr(SupplierInfo *spplr, unsigned short udpPort, const char *udpIp)
{
    spplr->udpPort = udpPort;
    spplr->udpFd = createUdpSckt(spplr->udpPort, udpIp, &spplr->udpAddr);
    spplr->tcpPort = genUniqueTcpPort();
    spplr->tcpFd = createTcpSckt(spplr->tcpPort);
    // rstrnt->name = uniqueUsrName(rstrnt->udpFd, rstrnt->tcpFd, rstrnt->udpAddr, rstrnt->tcpPort);
    spplr->name = uniqueUsrName(spplr->udpFd, spplr->tcpFd, spplr->udpAddr, spplr->tcpPort);
    // spplr->name = uniqueUsrName(spplr->tcpPort, spplr->udpAddr, spplr->udpFd, spplr->tcpFd);
    spplr->status = Free;
    NAME = spplr->name;
    logFile(NAME, "Initiated\n");
}

void initStore(SupplierInfo *spplr)
{
    spplr->allIngredients = getIngridients();
}

void cmdLineInterface(SupplierInfo *spplr, const char *cmd)
{
    logFile(NAME, "Command line interface started\n");
    if (cmd == NULL || strlen(cmd) == 0)
        return;
    else if (strcmp(cmd, ANSWER_REQUEST) == 0 && spplr->status == Busy)
    {
        spplr->status = Busy;
        char buff[INP_BUFF_SIZE];
        memset(buff, '\0', INP_BUFF_SIZE);
        setPromptReadInput(STDOUT_FILENO, "your answer(yes/no): ", buff, INP_BUFF_SIZE);
        char *encMsg = encodeMsg(creatMsg(spplr->name, spplr->tcpPort, buff, ANSWER_REQUEST));
        int a = send(spplr->currDmndrFd, encMsg, strlen(encMsg), 0);
        spplr->status = Free;
    }
    else if (strcmp(cmd, ANSWER_REQUEST) == 0 && spplr->status == Free)
    {
        setPromptReadInput(STDOUT_FILENO, "you have no request to answer!\n", NULL, 0);
    }
    else
    {
        setPromptReadInput(STDOUT_FILENO, "invalid command\n", NULL, 0);
    }
}

int udbHndlr(Message *msg, SupplierInfo *spplr)
{
    logFile(NAME, "UDP message received\n");
    char buff[INP_BUFF_SIZE];
    if ((strcmp(msg->title, CHECK_USERNAME_COMMAND) == 0) && msg->senderTcpPort != spplr->tcpPort)
    {
        logFile(NAME, "CHECK_USERNAME_COMMAND received\n");
        if (strcmp(spplr->name, msg->senderName) != 0)
            return 0;
        Message bcMsg = creatMsg(spplr->name, spplr->tcpPort, CHECK_USERNAME_COMMAND, CHECK_USERNAME_COMMAND);
        int serverFd = connectTcpClient(msg->senderTcpPort);
        char *encoded = encodeMsg(bcMsg);
        send(serverFd, encoded, strlen(encoded), 0);
    }
    if (strcmp(msg->title, SHOW_SUPPLIERS) == 0)
    {
        logFile(NAME, "SHOW_SUPPLIERS received\n");
        int serverFd = connectTcpClient(msg->senderTcpPort);
        Message tcpMsg = creatMsg(spplr->name, spplr->tcpPort, SHOW_SUPPLIERS, SHOW_SUPPLIERS);
        char *encoded = encodeMsg(tcpMsg);
        int a = send(serverFd, encoded, strlen(encoded), 0);
    }
    return 0;
}

int tcpHndlr(SupplierInfo *spplr, Message *msg)
{
    logFile(NAME, "TCP message received\n");
    if (strcmp(msg->title, REQUEST_INGREDIENT) == 0 && spplr->status == Free)
    {
        logFile(NAME, "REQUEST_INGREDIENT received\n");
        setPromptReadInput(STDOUT_FILENO, "new request ingredient!\n", NULL, 0);
        spplr->currDmndrFd = atoi(msg->content);
        spplr->status = Busy;
    }
    else if (strcmp(msg->title, REQUEST_INGREDIENT) == 0 && spplr->status == Busy)
    {
        logFile(NAME, "REQUEST_INGREDIENT received\n");
        int fd = connectTcpClient(msg->senderTcpPort);
        char *encMsg = encodeMsg(creatMsg(spplr->name, spplr->tcpPort, NO, ANSWER_REQUEST));
        send(fd, encMsg, strlen(encMsg), 0);
    }
    else if (strcmp(msg->title, TIMEOUT_MESSAGE) == 0 && spplr->status == Busy)
    {
        logFile(NAME, "REQUEST_INGREDIENT Timed out\n");
        setPromptReadInput(STDOUT_FILENO, "Request ingredient timed out!\n", NULL, 0);
        spplr->status = Free;
    }
    return 0;
}

int main(int argc, char const *argv[])
{
    int filFd = open("Supplier", O_RDONLY);

    SupplierInfo spplr;
    initSpplr(&spplr, atoi(argv[1]), IP_ADDR);
    initStore(&spplr);
    welcome(spplr.name, Supplier);

    fd_set masterSet, workingSet;
    struct sockaddr_in addrOut;

    int maxSd = (spplr.tcpFd > spplr.udpFd) ? spplr.tcpFd : spplr.udpFd;
    FD_ZERO(&masterSet);
    FD_SET(spplr.udpFd, &masterSet);
    FD_SET(spplr.tcpFd, &masterSet);
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
            if (i == spplr.udpFd)
            {
                recv(spplr.udpFd, buffer, INP_BUFF_SIZE, 0);
                Message msg = decodeMsg(buffer);
                udbHndlr(&msg, &spplr);
            }
            else if (i == spplr.tcpFd)
            {
                int new_socket = accTcpClient(spplr.tcpFd);
                FD_SET(new_socket, &masterSet);
                if (new_socket > maxSd)
                    maxSd = new_socket;
            }
            else if (i == STDIN_FILENO)
            {
                setPromptReadInput(STDIN_FILENO, NULL, buffer, INP_BUFF_SIZE);
                cmdLineInterface(&spplr, buffer);
            }
            else
            {
                recv(i, buffer, INP_BUFF_SIZE, 0);
                Message msg = decodeMsg(buffer);
                tcpHndlr(&spplr, &msg);
            }
        }
    }
    close(filFd);
}
