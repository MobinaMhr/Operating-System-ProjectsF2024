#include "billProcessor.hpp"

void BillProcessor::setCoeffs(std::string type)
{
    peakHourCoeff_ = (type == "Gas") ? 1.0 : 1.25;
    underMeanCoeff_ = (type == "Elictricity") ? 0.75 : 1.0;
}

void BillProcessor::run()
{
    log(ss, "bill processor successfully entered", color::MAGENTA);
    std::string msg;
    std::string cost;
    std::string buildingName;
    std::string facilityName;

    int counter = 0;
    while (counter < 3)
    {
        msg = server_->receive();
        if (msg != "")
        {
            log(ss, "bill processor received msg from named pipe", color::MAGENTA);
            Message cData = EncoderDecoder::decodeCostMsg(msg);

            size_t pos = cData.path.find('/');
            buildingName = cData.path.substr(0, pos);
            facilityName = cData.path.substr(pos + 1);

            setCoeffs(facilityName);

            MonthlyUsage mU = EncoderDecoder::decodeMonthlyUsage(cData.data);
            cost = calcBill(mU, cData.avgConsumpn, cData.pHour);

            client_ = std::make_shared<NamedPipeClient>(buildingName);
            client_->send(cost);
            log(ss, "bill processor sent msg to named pipe", color::MAGENTA);
            counter++;
        }
    }
}

BillProcessor::~BillProcessor()
{
    NamedPipe::remove_pipe("bill");
}

BillProcessor::BillProcessor()
{
    adr = "bill";
    server_ = std::make_shared<NamedPipeServer>(adr);
    client_ = nullptr;
}

std::string BillProcessor::calcBill(MonthlyUsage u, float avgC, int pH)
{
    float cost = 0.0;
    for (int d = 0; d < 30; d++)
    {
        for (int h = 0; h < 6; h++)
        {
            if (h == pH)
                cost += u[d][h] * peakHourCoeff_;
            else if (u[d][h] < avgC)
                cost += u[d][h] * underMeanCoeff_;
            else
                cost += u[d][h];
        }
    }
    return std::to_string(cost);
}

void BillProcessor::setDirName(const std::string &dir_name)
{
    dir_name_ = dir_name;
}