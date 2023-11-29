#ifndef FACILITY_PROCESSOR_HPP
#define FACILITY_PROCESSOR_HPP

#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdexcept>
#include <string>
#include <vector>
#include <ostream>
#include <iostream>
#include <fstream>
#include <sstream>
#include <numeric>
#include <algorithm>

#include "consts.hpp"
#include "encoderDecoder.hpp"
#include "unnamedPipe.hpp"

class FacilityProcessor
{
public:
    FacilityProcessor();
    void setDirName(const std::string &dir_name);
    std::string run(const std::string filePath);
    std::string encodeData();
    std::vector<int> extractPeakHour(Usage u);
    std::vector<std::vector<float>> calcDiffPeachAvg(Usage u);

private:
    std::string dir_name_;
    Usage u;
    std::vector<int> peakHours_;
    std::vector<float> avgUsage_;
    std::vector<std::vector<float>> diffPeakAvg_;
    std::vector<float> totalUsage_;
    std::stringstream ss;
};

#endif
