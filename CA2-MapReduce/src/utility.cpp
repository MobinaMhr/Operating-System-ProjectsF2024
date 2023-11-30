#include "utility.hpp"

std::vector<std::string> split(const std::string &str)
{
    std::vector<std::string> result;
    std::istringstream sstr(str);
    std::string item;
    while (std::getline(sstr, item, DELIM_CSV))
        result.push_back(std::move(item));
    return result;
}

CsvData readCsv(const std::string &filename)
{
    CsvData csv;
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::string errMsg = "Error opening file: " + filename + '\n';
        std::cerr << errMsg << std::endl;
        std::exit(1);
    }

    std::string line;
    std::getline(file, line);
    while (std::getline(file, line))
    {
        std::vector<std::string> splitted = split(line);
        csv.push_back(splitted);
    }
    return csv;
}

Usage extractData(CsvData readData)
{
    Usage usage;
    int index = 0;
    int month = 0;
    while (month < 12)
    {
        MonthlyUsage mUsage;
        int day = 0;
        while (day < 30)
        {
            DailyUsage dUsage;
            int hour = 0;
            while (hour < 6)
            {
                float u;
                index = month * 30 + day;
                u = std::stod(readData[index][hour + 3]);
                dUsage.push_back(u);
                hour++;
            }
            day++;
            mUsage.push_back(dUsage);
        }
        month++;
        usage.push_back(mUsage);
    }
    return usage;
}

bool containsSubstring(const std::string &input, const std::string &substring)
{
    return input.find(substring) != std::string::npos;
}

int argMax(std::vector<float> vect)
{
    auto maxItr = std::max_element(vect.begin(), vect.end());
    return std::distance(vect.begin(), maxItr);
}

void log(std::stringstream &ss, const std::string &message, std::string c)
{
    ss << c << message << color::RESET << std::endl;
    std::cerr << ss.str();
    ss.str("");
    ss.clear();
}

void log_(std::stringstream &ss, const std::string &message, std::string c)
{
    ss << c << message << color::RESET;
    std::cerr << ss.str();
    ss.str("");
    ss.clear();
}

void displayHelpPrompt()
{
    std::stringstream ss;
    log(ss, "Welcome to the Building Management System!", color::BLUE);
    log(ss, "----------------------------------------", color::BLACK);
    log(ss, "To use the program, follow these steps:", color::BLUE);
    log(ss, "First of all, the name of each building is shown .", color::BLUE);
    log(ss, "1. Enter one or more facility names separated by spaces.", color::BLUE);
    log(ss, "2. Press Enter.", color::BLUE);
    log(ss, "3. Enter the month id you want to see.", color::BLUE);
    log(ss, "4. Press Enter.", color::BLUE);
    log(ss, "\n", color::BLUE);
    log(ss, "Note: All inputs are case-sensitive.", color::BLUE);
    log(ss, "----------------------------------------", color::BLACK);
    log(ss, "Statistical indexes:", color::BLUE);
    log(ss, "[MEAN_CONSUMPTION] [TOTAL_CONSUMPTION] [TOTAL_COST] [PEAK_HOUR] [DELTA_PEAKH_AVG]", color::BLUE);
}