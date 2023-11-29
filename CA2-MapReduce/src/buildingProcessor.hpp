#ifndef BUILDING_PROCESSOR_HPP
#define BUILDING_PROCESSOR_HPP

#include <ostream>
#include <iostream>
#include <string>
#include <vector>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sstream>
#include <map>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <iostream>
#include <algorithm>
#include <cstring>
#include <string>
#include <memory>
#include <cmath>

#include "consts.hpp"
#include "encoderDecoder.hpp"
#include "unnamedPipe.hpp"
#include "namedPipe.hpp"

class BuildingProcessor
{
public:
    BuildingProcessor();

    void setDirName(const std::string &dir_name);

    void setBuildingName();

    std::string getCost(MonthlyUsage usage, float avgUsage, std::string path);

    std::string writeReport(const std::string &userInput, Facility f,
                            int m, Usage usag, std::string fName);

    void decodeData(std::string encodedString, int id);

    void addToEncUsgs(std::string res, int fId, std::string path);

    void decodeData(std::string encodedString);

    void getFacilityDetails();

    void sendReportContent();

    void setFds(std::string arg1, std::string arg2);

    std::string setFacilityDetail(std::string fName, Usage usage, Facility f,
                                  std::string order, std::vector<int> months);

    void getReportOrder();

    ~BuildingProcessor();

private:
    std::vector<std::string> facilityNames;
    std::vector<Facility> facilities;
    std::vector<std::string> encodedPathUsages;
    std::vector<Usage> usages;
    std::string dir_name_;
    std::string buildingName;
    int rFd;
    int wFd;
    Req r;
    std::shared_ptr<NamedPipeServer> server_;
    std::shared_ptr<NamedPipeClient> client_;
    void fetchBuildings();
    void fetchFacilities();
    void createBillProcessor();
    std::stringstream ss;
};

#endif
