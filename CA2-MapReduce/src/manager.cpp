#include "manager.hpp"
#include <iostream>
#include <string>

ManagementSystem::ManagementSystem() : buildingCounter(0) {}

void ManagementSystem::setDirName(const std::string &dir_name)
{
    dir_name_ = dir_name;
}

int ManagementSystem::findBuildingFds(std::string target)
{
    auto it = std::find(buildings.begin(), buildings.end(), target);
    int index = -1;

    if (it != buildings.end())
        index = std::distance(buildings.begin(), it);
    return index;
}

void getInput(std::string name, int id, InputCmd &c)
{
    std::stringstream ss;
    c.buildingName = name;
    std::vector<std::string> clr = {color::CYAN, color::MAGENTA};
    log_(ss, "Enter facility Names separated by spaces: \n> ", clr[id % 2]);
    std::getline(std::cin, c.facilityName);

    log_(ss, "Enter month ID: \n> ", clr[id % 2]);
    std::getline(std::cin, c.monthIdId);

    log_(ss, "What statistical index you prefere to get (separated by spaces): \n> ",
         clr[id % 2]);
    std::getline(std::cin, c.statisticalIndex);
}

void ManagementSystem::dispReport(InputCmd c, int id)
{
    std::string report = "";
    std::string msg = EncoderDecoder::encode(c.facilityName, c.statisticalIndex, c.monthIdId);
    msg += '\0';
    write(fds[id][0][1], msg.c_str(), msg.size());
    close(fds[id][0][1]);
    log(ss, "report request send to unnamed pipe (to child)\n", color::BLACK);

    report += readFd(fds[id][1][0]);
    close(fds[id][1][0]);
    log(ss, "report received from unnamed pipe (from child)\n", color::BLACK);
    report.append("\n\n");
    std::cout << report << std::endl;
}

void ManagementSystem::getCmd()
{
    int buildingId = 0;
    std::string command = "";

    for (auto buildingName : buildings)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        log(ss, "\n" + std::to_string(buildingId + 1) + ". " + buildingName + "\n",
            color::WHITE);
        InputCmd c;
        getInput(buildingName, buildingId + 1, c);
        createBuilding(buildingName);
        dispReport(c, buildingId);
        buildingId++;
    }
}

void ManagementSystem::run()
{
    fetchBuildings();
    dispFacilities();
    displayHelpPrompt();
    createBillProcess();
    getCmd();

    wait(NULL);
    wait(NULL);
    wait(NULL);
    wait(NULL);
}

void ManagementSystem::fetchBuildings()
{
    DIR *dir;
    struct dirent *entry;

    if ((dir = opendir(dir_name_.c_str())) == nullptr)
    {
        closedir(dir);
        throw std::runtime_error("Error opening directory");
    }

    while ((entry = readdir(dir)) != nullptr)
    {
        if ((entry->d_type == DT_DIR) && (std::string(entry->d_name) != ".") && (std::string(entry->d_name) != ".."))
        {
            buildingCounter++;
            buildings.push_back(entry->d_name);
        }
    }
    closedir(dir);
}

void ManagementSystem::dispFacilities()
{
    log(ss, "\nFacility list:\n", color::WHITE);
    log(ss, "1. Gas\n2. Water\n3. Electricity\n", color::WHITE);
}

ManagementSystem::~ManagementSystem()
{
    for (auto &fd : fds)
    {
        for (auto &fdType : fd)
        {
            for (int f : fdType)
            {
                close(f);
            }
        }
    }
}

void ManagementSystem::createBillProcess()
{
    log(ss, "in the management System creating billProcess", color::WHITE);
    int pid = fork();
    if (pid == 0)
        execl(BILL_PROCESS, BILL_PROCESS, dir_name_.c_str(), (char *)NULL);
    else if (pid < 0)
        throw std::runtime_error("fork() failed in makeCounterProcesses.");
}

void ManagementSystem::createBuilding(std::string name)
{
    log(ss, "in the management System creating building " + name, color::WHITE);
    std::string buildingPath = dir_name_ + SLASH + name;

    std::vector<int> readFds = createPipe();
    std::vector<int> writeFds = createPipe();

    int id = fds.size();
    FdType fd;
    fds.push_back({{readFds[0], readFds[1]}, {writeFds[0], writeFds[1]}});

    int pid = fork();
    if (pid == 0)
    {
        execl(BUILDING_PROCESS, BUILDING_PROCESS, buildingPath.c_str(),
              std::to_string(fds[id][0][0]).c_str(), std::to_string(fds[id][1][1]).c_str(), (char *)NULL);
    }
    else if (pid < 0)
        throw std::runtime_error("fork() failed in makeCounterProcesses.");
}

/// write in fds[id].read[1] main ||||||
/// read in fds[id].write[0] main ||||||