#include "facilityProcessor.hpp"

FacilityProcessor::FacilityProcessor()
    : avgUsage_(std::vector<float>(12, 0.0)),
      totalUsage_(std::vector<float>(12, 0.0)) {}

void FacilityProcessor::setDirName(const std::string &dir_name)
{
    dir_name_ = dir_name;
}

std::vector<std::vector<float>> FacilityProcessor::calcDiffPeachAvg(Usage u)
{
    std::vector<std::vector<float>> peakDiffAvg(12, std::vector<float>(30, 0.0));
    for (int month = 0; month < 12; month++)
    {
        int peakHour = peakHours_[month];
        for (int day = 0; day < 30; day++)
        {
            peakDiffAvg[month][day] = u[month][day][peakHour] - avgUsage_[month];
        }
    }
    return peakDiffAvg;
}

std::vector<int> FacilityProcessor::extractPeakHour(Usage u)
{
    float totalU = 0.0;
    int day = 0;
    std::vector<int> pHVect(12, -1);
    for (int month = 0; month < 12; month++)
    {
        totalU = 0;
        day = 0;
        std::vector<float> hours(6, 0.0);
        while (day < 30)
        {
            for (int h = 0; h < 6; h++)
            {
                totalU += u[month][day][h];
                hours[h] += u[month][day][h];
            }
            day++;
        }

        pHVect[month] = argMax(hours);

        totalUsage_[month] = totalU;
        avgUsage_[month] = (totalU / 180);
    }
    return pHVect;
}

std::string FacilityProcessor::encodeData()
{
    std::string encodedString, encodedUsage;
    encodedString = EncoderDecoder::encode(peakHours_, avgUsage_, totalUsage_, diffPeakAvg_);
    encodedUsage = EncoderDecoder::encodeUsage(u);
    return encodedString + STAR + encodedUsage;
}

std::string FacilityProcessor::run(const std::string filePath)
{
    log(ss, "facility processor successfully entered." , color::GREEN);
    CsvData readData = readCsv(filePath);
    log(ss, "facility processor successfully read csv file." , color::GREEN);
    u = extractData(readData);
    peakHours_ = extractPeakHour(u);
    diffPeakAvg_ = calcDiffPeachAvg(u);
    return encodeData();
}
