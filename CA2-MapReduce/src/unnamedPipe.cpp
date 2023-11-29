#include "unnamedPipe.hpp"

std::vector<int> createPipe()
{
    int fd[2];
    std::vector<int> res;
    if (pipe(fd) == -1)
    {
        std::stringstream ss;
        throw std::runtime_error("Failed to create pipe");
    }
    res.push_back(fd[0]);
    res.push_back(fd[1]);
    return res;
}

void closeFd(int fd)
{
    if (close(fd) == -1)
        throw std::runtime_error("Failed to close the pipe");
}

int writeFd(const char *msg, int size, int wFd)
{
    int bytesWritten = write(wFd, msg, size);
    if (bytesWritten == -1)
        throw std::runtime_error("Failed to write to pipe");
    return bytesWritten;
}

std::string readFd(int rFd)
{
    std::string res;
    char buff[1024];
    while (true)
    {
        int read_bytes = read(rFd, buff, 1024);
        if (read_bytes == -1)
        {
            if (errno == EAGAIN)
                break;
            else
                throw std::runtime_error("Failed to read from pipe");
        }
        else if (read_bytes < 1024)
        {
            res.append(buff, read_bytes);
            break;
        }
        res.append(buff, read_bytes);
    }
    return res;
}
