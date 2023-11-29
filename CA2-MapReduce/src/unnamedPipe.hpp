#include <vector>
#include <iostream>
#include <filesystem>
#include <vector>
#include <map>
#include <unistd.h>
#include <fstream>
#include <cstring>
#include <sys/wait.h>
#include <sstream>
#include <string>

#include "utility.hpp"

std::vector<int> createPipe();

void closeFd(int fd);

int writeFd(const char *msg, int size, int wFd);

std::string readFd(int rFd);
