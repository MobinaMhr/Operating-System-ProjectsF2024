#ifndef __NAMED_PIPE_H__
#define __NAMED_PIPE_H__

#include <string>
#include <iostream>

class NamedPipe
{
public:
    NamedPipe(const std::string &pName);

    static void remove_pipe(const std::string &pPath);

protected:
    std::string pName_;
    int pFd_;
};

class NamedPipeClient : NamedPipe
{
public:
    NamedPipeClient(const std::string &pName);

    void send(const std::string &msg);

    ~NamedPipeClient();
};

class NamedPipeServer : NamedPipe
{
public:
    NamedPipeServer(const std::string &pName);

    std::string receive();

    ~NamedPipeServer();
};

#endif
