#ifndef CONSTS_HPP
#define CONSTS_HPP

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <iostream>

using DailyUsage = std::vector<float>;
using MonthlyUsage = std::vector<DailyUsage>;
using Usage = std::vector<MonthlyUsage>;
using CsvData = std::vector<std::vector<std::string>>;
using FdType = std::vector<std::vector<int>>;
using buildingName = std::string;

const std::string BUILDINGS_PATH = "./buildings";
const std::string STAR = "*";
const std::string SLASH = "/";

const char UP_ARROW[] = u8"\u2191";
const char DOWN_ARROW[] = u8"\u2193";
const char BILL_PROCESS[] = "./billProcess.out";
const char FACILITY_PROCESS[] = "./facilityProcess.out";
const char DELIM_CSV = ',';
const char BUILDING_PROCESS[] = "./buildingProcess.out";

namespace color
{
    const std::string RESET = "\033[0m";
    const std::string BLACK = "\033[30m";
    const std::string RED = "\033[31m";
    const std::string GREEN = "\033[32m";
    const std::string YELLOW = "\033[33m";
    const std::string BLUE = "\033[34m";
    const std::string MAGENTA = "\033[35m";
    const std::string CYAN = "\033[36m";
    const std::string WHITE = "\033[37m";
}

const std::string monthMap[] = {"January", "February", "March", "April",
                                "May", "June", "July", "August", "September",
                                "October", "November", "December"};

struct Message
{
    std::string path;
    std::string data;
    float avgConsumpn;
    int pHour;
};

struct MonthlyDetail
{
    int peakHour;
    float meanConsumption;
    float billAmount;
    float totalConsumption;
};

struct InputCmd
{
    std::string buildingName;
    std::string facilityName;
    std::string statisticalIndex;
    std::string monthIdId;
};

struct Facility
{
    std::vector<int> peakHours;
    std::vector<float> avgUsage;
    std::vector<std::vector<float>> diffPeakAvg;
    std::vector<float> totalUsage;
};

struct Req
{
    std::string fName;
    std::string order;
    std::string monthId;
};

#endif
