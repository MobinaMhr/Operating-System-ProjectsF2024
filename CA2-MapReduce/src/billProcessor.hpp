#ifndef BILL_PROCESSOR_HPP
#define BILL_PROCESSOR_HPP

#include <map>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <iostream>
#include <algorithm>
#include <string>
#include <memory>

#include "consts.hpp"
#include "encoderDecoder.hpp"
#include "namedPipe.hpp"
#include "utility.hpp"

class BillProcessor
{
public:
    BillProcessor();
    void setDirName(const std::string &dir_name);
    void run();
    void setCoeffs(std::string type);
    std::string calcBill(MonthlyUsage u, float avgC, int pH);
    ~BillProcessor();

private:
    std::shared_ptr<NamedPipeServer> server_;
    std::shared_ptr<NamedPipeClient> client_;
    std::string adr;
    float peakHourCoeff_;
    float underMeanCoeff_;
    void receiveMsg();
    std::string dir_name_;
    std::stringstream ss;
};

#endif
