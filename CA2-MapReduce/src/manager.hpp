#ifndef MANAGER_HPP
#define MANAGER_HPP

#include <map>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <unistd.h>
#include <filesystem>
#include <sys/wait.h>
#include <iostream>
#include <algorithm>
#include <stdexcept>
#include <thread>
#include <chrono>
#include <cstring>
#include <dirent.h>

#include "consts.hpp"
#include "encoderDecoder.hpp"
#include "unnamedPipe.hpp"
// #include "utility.hpp"

class ManagementSystem
{
public:
    ManagementSystem();

    void run();

    void getBuildingName();

    void setDirName(const std::string &dir_name);

    void createBuilding(std::string name);

    void createBillProcess();

    void dispFacilities();

    void dispReport(InputCmd c, int id);

    ~ManagementSystem();

private:
    std::vector<FdType> fds;
    std::string dir_name_;
    int buildingCounter;
    std::vector<buildingName> buildings;
    void fetchBuildings();
    int findBuildingFds(std::string target);
    void getCmd();
    std::vector<InputCmd> commands;
    std::stringstream ss;
};

#endif
