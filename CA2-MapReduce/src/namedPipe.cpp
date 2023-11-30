#include "namedPipe.hpp"

#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdexcept>
#include <string>

NamedPipe::NamedPipe(const std::string &pName)
    : pName_(pName) {}

NamedPipeClient::~NamedPipeClient()
{
    close(pFd_);
}

NamedPipeServer::~NamedPipeServer()
{
    close(pFd_);
}

void NamedPipe::remove_pipe(const std::string &pPath)
{
    remove(pPath.c_str());
}

NamedPipeClient::NamedPipeClient(const std::string &pName)
    : NamedPipe(pName)
{
    do
    {
        pFd_ = open(pName.c_str(), O_WRONLY);
    } while (pFd_ == -1);
}

NamedPipeServer::NamedPipeServer(const std::string &pName)
    : NamedPipe(pName)
{
    if ((pFd_ = open(pName.c_str(), O_RDONLY | O_NONBLOCK)) == -1)
    {
        if (mkfifo(pName.c_str(), 0777) != 0)
            throw std::runtime_error("Erro in making FIFO file.");
        else
            pFd_ = open(pName_.c_str(), O_RDONLY | O_NONBLOCK);
    }
    if (pFd_ == -1)
        throw std::runtime_error("Couldn't open server side of named pipe.(" + pName + ")");
}

void NamedPipeClient::send(const std::string &msg)
{
    if (write(pFd_, msg.c_str(), msg.size()) != int(msg.size()))
        throw std::runtime_error("Error on sending the whole msg over named pipe.");
}

std::string NamedPipeServer::receive()
{
    std::string res;
    char buff[4096];
    while (true)
    {
        int readBytes = read(pFd_, buff, 4096);
        if (readBytes == -1)
            break;
        if (readBytes < 4096)
        {
            res.append(buff, readBytes);
            break;
        }
        res.append(buff, readBytes);
    }
    return res;
}