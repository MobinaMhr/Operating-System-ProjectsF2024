#include "encoderDecoder.hpp"

#include <iostream>
#include <sstream>
#include <vector>

std::string EncoderDecoder::encodeCostMsg(const Message &message)
{
    std::ostringstream oss;
    oss << message.path << STAR
        << message.data << STAR
        << message.avgConsumpn << STAR
        << message.pHour;
    return oss.str();
}

Message EncoderDecoder::decodeCostMsg(const std::string &encodedString)
{
    Message message;
    std::istringstream iss(encodedString);

    // Assuming the fields are separated by '*'
    std::getline(iss, message.path, '*');
    std::getline(iss, message.data, '*');
    iss >> message.avgConsumpn;
    iss.ignore(); // Consume the '*'
    iss >> message.pHour;

    return message;
}

CsvData EncoderDecoder::csvDecoder(const std::string &encodedMsg)
{
    CsvData decodedMsg;

    std::istringstream iss(encodedMsg);
    std::string line;

    while (std::getline(iss, line, '|'))
    {
        std::istringstream lineStream(line);
        std::string field;
        std::vector<std::string> rowData;

        while (std::getline(lineStream, field, '-'))
            rowData.push_back(field);

        decodedMsg.push_back(rowData);
    }

    return decodedMsg;
}

std::string EncoderDecoder::encode(const std::string &d1,
                                   const std::string &d2,
                                   const std::string &d3)
{
    return d1 + "|" + d2 + "|" + d3;
}

std::string EncoderDecoder::encodeMonthlyUsage(const MonthlyUsage &monthlyUsage)
{
    std::ostringstream oss;

    for (const auto &dailyUsage : monthlyUsage)
    {
        for (const auto &value : dailyUsage)
        {
            oss << value << ' '; // Use space as the separator
        }
        oss << '\n'; // Use newline to separate days
    }

    return oss.str();
}

MonthlyUsage EncoderDecoder::decodeMonthlyUsage(const std::string &encodedString)
{
    MonthlyUsage monthlyUsage;

    std::istringstream iss(encodedString);
    std::string line;

    while (std::getline(iss, line))
    {
        DailyUsage dailyUsage;
        std::istringstream lineStream(line);
        float value;

        while (lineStream >> value)
        {
            dailyUsage.push_back(value);
        }

        monthlyUsage.push_back(dailyUsage);
    }

    return monthlyUsage;
}

std::string EncoderDecoder::encodeUsage(const Usage &usage)
{
    std::stringstream ss;
    for (const auto &monthlyUsage : usage)
    {
        for (const auto &dailyUsage : monthlyUsage)
        {
            for (const auto &value : dailyUsage)
            {
                ss << value << ',';
            }
            ss << ';'; // Use ';' to separate dailyUsage
        }
        ss << '|'; // Use '|' to separate MonthlyUsage
    }

    return ss.str();
}

Usage EncoderDecoder::decodeUsage(const std::string &str)
{
    Usage usage;
    MonthlyUsage monthlyUsage;
    DailyUsage dailyUsage;

    std::istringstream ss(str);
    std::string token;

    while (getline(ss, token, '|'))
    {
        std::istringstream monthlyStream(token);
        while (getline(monthlyStream, token, ';'))
        {
            std::istringstream dailyStream(token);
            float value;
            DailyUsage dailyUsage;

            while (dailyStream >> value)
            {
                dailyUsage.push_back(value);
                if (dailyStream.peek() == ',')
                    dailyStream.ignore();
            }

            monthlyUsage.push_back(dailyUsage);
        }

        usage.push_back(monthlyUsage);
        monthlyUsage.clear();
    }

    return usage;
}

void EncoderDecoder::decode(const std::string &msg,
                            std::string &d1,
                            std::string &d2,
                            std::string &d3)
{
    size_t pos1 = msg.find('|');
    size_t pos2 = msg.find('|', pos1 + 1);

    d1 = msg.substr(0, pos1);
    d2 = msg.substr(pos1 + 1, pos2 - pos1 - 1);
    d3 = msg.substr(pos2 + 1);
}

std::string EncoderDecoder::encode(const std::vector<int> &peakHours,
                                   const std::vector<float> &avgUsage,
                                   const std::vector<float> &totalUsage,
                                   const std::vector<std::vector<float>> &diffPeakAvg)
{
    std::ostringstream oss;
    encodeVector(oss, peakHours);
    encodeVector(oss, avgUsage);
    encodeVector(oss, totalUsage);
    encodeNestedVector(oss, diffPeakAvg);
    return oss.str();
}

void EncoderDecoder::decode(const std::string &encodedString,
                            std::vector<int> &peakHours,
                            std::vector<float> &avgUsage,
                            std::vector<float> &totalUsage,
                            std::vector<std::vector<float>> &diffPeakAvg)
{
    for (int i = 0; i < 12; i++)
    {
        std::istringstream iss(encodedString);
        decodeVector(iss, peakHours);
        decodeVector(iss, avgUsage);
        decodeVector(iss, totalUsage);
        decodeNestedVector(iss, diffPeakAvg);
    }
}

void EncoderDecoder::encodeVector(std::ostringstream &oss, const std::vector<int> &vec)
{
    for (size_t i = 0; i < vec.size(); ++i)
    {
        oss << vec[i];
        if (i < vec.size() - 1)
        {
            oss << ",";
        }
    }
    oss << ";";
}

void EncoderDecoder::encodeVector(std::ostringstream &oss, const std::vector<float> &vec)
{
    for (size_t i = 0; i < vec.size(); ++i)
    {
        oss << vec[i];
        if (i < vec.size() - 1)
        {
            oss << ",";
        }
    }
    oss << ";";
}

void EncoderDecoder::encodeNestedVector(std::ostringstream &oss, const std::vector<std::vector<float>> &vec)
{
    for (size_t i = 0; i < vec.size(); ++i)
    {
        encodeVector(oss, vec[i]);
    }
    oss << "|";
}

void EncoderDecoder::decodeVector(std::istringstream &iss, std::vector<int> &vec)
{
    decodeValues(iss, vec);
}

void EncoderDecoder::decodeVector(std::istringstream &iss, std::vector<float> &vec)
{
    decodeValues(iss, vec);
}

void EncoderDecoder::decodeNestedVector(std::istringstream &iss, std::vector<std::vector<float>> &vec)
{
    std::string token;
    while (std::getline(iss, token, '|'))
    {
        std::istringstream innerIss(token);
        std::vector<float> innerVec;
        decodeValues(innerIss, innerVec);
        vec.push_back(innerVec);
    }
}

template <typename T>
void EncoderDecoder::decodeValues(std::istringstream &iss, std::vector<T> &vec)
{
    std::string token;
    std::getline(iss, token, ';');
    std::istringstream tokenStream(token);
    T value;
    while (tokenStream >> value)
    {
        vec.push_back(value);
        if (tokenStream.peek() == ',')
        {
            tokenStream.ignore();
        }
    }
}
