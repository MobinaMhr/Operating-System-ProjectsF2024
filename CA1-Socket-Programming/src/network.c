#include "network.h"

// int genTcp(struct sockaddr_in *addrOut)
// {
//     int tcpFd = socket(AF_INET, SOCK_STREAM, 0);

//     struct sockaddr_in addr;

//     int reuseport = 1;
//     setsockopt(tcpFd, SOL_SOCKET, SO_REUSEPORT, &reuseport, sizeof(reuseport));

//     addr.sin_family = AF_INET;
//     addr.sin_port = htons(0);
//     addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

//     bind(tcpFd, (struct sockaddr *)&addr, sizeof(addr));

//     socklen_t addrLen = sizeof(addr);

//     getsockname(tcpFd, (struct sockaddr *)&addr, &addrLen);

//     if (listen(tcpFd, 4) == -1)
//     {
//         write(2, "listning failed!\n", strlen("listening failed!\n"));
//         exit(1);
//     }

//     *addrOut = addr;

//     return tcpFd;
// }

// int genUdp(struct sockaddr_in *addrOut, unsigned short port)
// {
//     int bcFd = socket(AF_INET, SOCK_DGRAM, 0);

//     if (bcFd < 0)
//     {
//         write(2, "generating udp socket failed\n", strlen("generating  udp socket failed\n"));
//         exit(1);
//     }

//     struct sockaddr_in addr;
//     int broadcast = 1;
//     setsockopt(bcFd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));
//     int reuseport = 1;
//     setsockopt(bcFd, SOL_SOCKET, SO_REUSEPORT, &reuseport, sizeof(reuseport));

//     addr.sin_family = AF_INET;
//     addr.sin_port = htons(port);
//     addr.sin_addr.s_addr = inet_addr("255.255.255.255");

//     *addrOut = addr;

//     if (bind(bcFd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
//     {
//         write(2, "bind failed\n", strlen("bind failed\n"));
//         close(bcFd);
//         exit(1);
//     };

//     return bcFd;
// }

int createUdpSckt(const int port, const char *udpIp, struct sockaddr_in *addr)
{
    struct sockaddr_in bc_address;
    int broadcast = 1, reuse_port = 1;

    int socketFd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socketFd == -1)
    {
        setPromptReadInput(STDOUT_FILENO, "err in socket\n", NULL, 0);
    }
    setsockopt(socketFd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));
    setsockopt(socketFd, SOL_SOCKET, SO_REUSEPORT, &reuse_port, sizeof(reuse_port));
    bc_address.sin_family = AF_INET;
    bc_address.sin_port = htons(port);
    bc_address.sin_addr.s_addr = inet_addr("255.255.255.255");
    if (bind(socketFd, (struct sockaddr *)&bc_address, sizeof(bc_address)) < 0)
    {
        setPromptReadInput(STDERR_FILENO, "bind failed baba\n", NULL, 0);
        exit(EXIT_FAILURE);
    }
    *addr = bc_address;

    return socketFd;
}

int createTcpSckt(int port)
{
    struct sockaddr_in address;
    int fd = socket(AF_INET, SOCK_STREAM, 0);

    int opt = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(fd, (struct sockaddr *)&address, sizeof(address)) == -1)
    {
        close(fd);
        return -1;
    }

    listen(fd, 4);
    return fd;
}

int accTcpClient(int server_fd)
{
    struct sockaddr_in client_address;
    int address_len = sizeof(client_address);
    int fd = accept(server_fd, (struct sockaddr *)&client_address, (socklen_t *)&address_len);
    return fd;
}

int connectTcpClient(int port)
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_address;

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        write(STDOUT_FILENO, "Error in connecting to server\n", 31);
        return -1;
    }

    return fd;
}