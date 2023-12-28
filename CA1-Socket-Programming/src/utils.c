#include "utils.h"

void setPromptReadInput(int fd, const char *prmpt, char *dst, int dstLen)
{
    int readBit;
    if (prmpt != NULL)
        write(fd, prmpt, strlen(prmpt));
    if (dst != NULL)
    {
        readBit = read(fd, dst, dstLen);
        if (readBit <= 0)
        {
            setPromptReadInput(STDERR_FILENO, "didn't read name.\n", NULL, 0);
            exit(EXIT_FAILURE);
        }
        dst[readBit - 1] = '\0';
    }
}

void printResponse(const char *rstrntName, const char *response)
{
    char prmpt[100];
    sprintf(prmpt, "%s %s", rstrntName, response);
    setPromptReadInput(STDOUT_FILENO, prmpt, NULL, 0);
}

int genUniqueTcpPort()
{
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd == -1)
    {
        setPromptReadInput(STDERR_FILENO, "socket", NULL, 0);
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = 0;

    if (bind(sock_fd, (struct sockaddr *)&address, sizeof(address)) == -1)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    socklen_t len = sizeof(address);
    if (getsockname(sock_fd, (struct sockaddr *)&address, &len) == -1)
    {
        perror("getsockname");
        exit(EXIT_FAILURE);
    }

    unsigned short int port = ntohs(address.sin_port);
    close(sock_fd);
    return port;
}

void welcome(char *name, Client type)
{
    setPromptReadInput(STDOUT_FILENO, genWelcomeMsg(name, type), NULL, 0);
}

int find_unused_port()
{
    int scktFd = socket(AF_INET, SOCK_STREAM, 0);
    if (scktFd < 0)
    {
        perror("Socket creation failed");
        return -1;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = 0;

    if (bind(scktFd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("Socket binding failed\n");
        close(scktFd);
        return -1;
    }

    socklen_t addrLen = sizeof(addr);
    if (getsockname(scktFd, (struct sockaddr *)&addr, &addrLen) < 0)
    {
        perror("getsockname failed");
        close(scktFd);
        return -1;
    }

    int port = ntohs(addr.sin_port);
    close(scktFd);

    return port;
}

void consoleLock() {
    struct termios raw;
    tcgetattr(STDIN_FILENO, &raw);
    raw.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void consoleUnlock() {
    struct termios raw;
    tcgetattr(STDIN_FILENO, &raw);
    raw.c_lflag |= (ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void alarmHndlr(int sig)
{
    consoleUnlock();
    return;
}

Message creatMsg(char *name, unsigned short int tcpPort, char *cntnt, char *title)
{
    Message msg;
    msg.senderName = (char*)malloc(strlen(name));
    strcpy(msg.senderName, name);
    strcpy(msg.content, cntnt);
    strcpy(msg.title, title);
    msg.senderTcpPort = tcpPort;
    return msg;
}

char *uniqueUsrName(int udpFd, int tcpFd, struct sockaddr_in udpAddr, unsigned short tcpPort)
{
    char buffer[INP_BUFF_SIZE];
    Message tcpMsg;
    tcpMsg.senderTcpPort = tcpPort;
    strcpy(tcpMsg.title, CHECK_USERNAME_COMMAND);
    strcpy(tcpMsg.content, CHECK_USERNAME_COMMAND);

    while (1)
    {
        memset(buffer, '\0', INP_BUFF_SIZE);
        setPromptReadInput(STDIN_FILENO, SIGNIN_PROMPT, buffer, INP_BUFF_SIZE);
        tcpMsg.senderName = (char *)malloc(strlen(buffer));
        strcpy(tcpMsg.senderName, buffer);
        char *msg = encodeMsg(tcpMsg);
        sendto(udpFd, msg, strlen(msg), 0, (struct sockaddr *)&udpAddr, sizeof(udpAddr));
        signal(SIGALRM, alarmHndlr);
        siginterrupt(SIGALRM, 1);
        alarm(3);
        int fd_ = accTcpClient(tcpFd);
        alarm(0);
        memset(buffer, '\0', INP_BUFF_SIZE);
        recv(udpFd, buffer, INP_BUFF_SIZE, 0);
        if (fd_ < 0)
            break;
        setPromptReadInput(STDIN_FILENO, "\n", NULL, 0);
    }
    return tcpMsg.senderName;
}

void startDisply()
{
    setPromptReadInput(STDOUT_FILENO, "\n-----------------------", NULL, 0);
}

void endDisply()
{
    setPromptReadInput(STDOUT_FILENO, "\n-----------------------\n\n", NULL, 0);
}

void showUser(char *name, unsigned short int port)
{
    char line[INP_BUFF_SIZE];
    memset(line, '\0', INP_BUFF_SIZE);
    sprintf(line, "\n%s %u", name, port);
    setPromptReadInput(STDOUT_FILENO, line, NULL, 0);
}

char *encodeMsg(Message msg)
{
    char *result;
    result = (char *)malloc(INP_BUFF_SIZE * sizeof(char));
    sprintf(result, "%s-%u-%s-%s", msg.senderName, msg.senderTcpPort, msg.title, msg.content);
    result = (char *)realloc(result, strlen(result) + 1);
    return result;
}

Message decodeMsg(char *msg)
{
    char delimiter[] = "-";
    Message decodedMsg;
    decodedMsg.senderName = strtok(msg, "-");
    char *port = strtok(NULL, "-");
    decodedMsg.senderTcpPort = atoi(port);
    strcpy(decodedMsg.title, strtok(NULL, "-"));
    strcpy(decodedMsg.content, strtok(NULL, "-"));
    return decodedMsg;
}

char *genWelcomeMsg(char *username, Client type)
{
    const char *welcome = "Welcome ";
    const char *as = " as ";

    const char *restaurant = "Restaurant";
    const char *customer = "Customer";
    const char *supplier = "Supplier";

    char *msg = (char *)malloc(INP_BUFF_SIZE * sizeof(char));
    sprintf(msg, "%s%s%s%s!\n", welcome, username,
            as, type == Restaurant ? restaurant : type == Customer ? customer
                                                                   : supplier);
    return msg;
}


int logFile(const char* name, const char* txt) {
    char fname[100] = {'\0'};
    sprintf(fname, "%s%s", "./", name);
    strcat(fname, ".txt");

    chmod(fname, S_IWUSR | S_IRUSR);
    int fd = open(fname, O_CREAT | O_WRONLY | O_APPEND);
    if (fd < 0) return 1;

    if (write(fd, txt, strlen(txt)) < 0) return 1;
    close(fd);
    return 0;
}
