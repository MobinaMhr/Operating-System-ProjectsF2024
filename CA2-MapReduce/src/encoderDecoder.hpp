#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <vector>
#include <iostream>
#include <sstream>
#include <string>

#include "consts.hpp"

class EncoderDecoder
{
public:
    static std::string encode(const std::vector<int> &peakHours,
                              const std::vector<float> &avgUsage,
                              const std::vector<float> &totalUsage,
                              const std::vector<std::vector<float>> &diffPeakAvg);

    static void decode(const std::string &encodedString,
                       std::vector<int> &peakHours,
                       std::vector<float> &avgUsage,
                       std::vector<float> &totalUsage,
                       std::vector<std::vector<float>> &diffPeakAvg);

    static std::string encode(const std::string &d1,
                              const std::string &d2,
                              const std::string &d3);

    static void decode(const std::string &msg,
                       std::string &d1,
                       std::string &d2,
                       std::string &d3);

    static Usage decodeUsage(const std::string &str);
    static std::string encodeUsage(const Usage &usage);

    static std::string encodeCostMsg(const Message &message);
    static Message decodeCostMsg(const std::string &encodedString);

    static MonthlyUsage decodeMonthlyUsage(const std::string &encodedString);
    static std::string encodeMonthlyUsage(const MonthlyUsage &monthlyUsage);

    static CsvData csvDecoder(const std::string &encodedMsg);

private:
    static void encodeVector(std::ostringstream &oss, const std::vector<int> &vec);

    static void encodeVector(std::ostringstream &oss, const std::vector<float> &vec);

    static void encodeNestedVector(std::ostringstream &oss, const std::vector<std::vector<float>> &vec);

    static void decodeVector(std::istringstream &iss, std::vector<int> &vec);

    static void decodeVector(std::istringstream &iss, std::vector<float> &vec);

    static void decodeNestedVector(std::istringstream &iss, std::vector<std::vector<float>> &vec);

    template <typename T>
    static void decodeValues(std::istringstream &iss, std::vector<T> &vec);
};

#endif
