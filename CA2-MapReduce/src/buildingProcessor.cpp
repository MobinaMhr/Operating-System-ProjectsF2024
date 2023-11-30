#include "buildingProcessor.hpp"

BuildingProcessor::BuildingProcessor()
{
    facilityNames = {"Gas",
                     "Water",
                     "Electricity"};
    client_ = std::make_shared<NamedPipeClient>("bill");
}

void BuildingProcessor::setFds(std::string arg1, std::string arg2)
{
    rFd = stoi(arg1);
    wFd = stoi(arg2);
}

std::string BuildingProcessor::getCost(MonthlyUsage usage, float avgUsage, std::string path)
{
    std::string msg = EncoderDecoder::encodeMonthlyUsage(usage);
    msg = path + STAR + msg + STAR + std::to_string(avgUsage);
    client_->send(msg);
    log(ss, "building process sent msg to named pipe", color::YELLOW);
    std::string cost = "";
    for (;;)
    {
        cost = server_->receive();
        if (cost != "")
        {
            log(ss, "building process received msg from named pipe", color::YELLOW);
            return cost;
        }
    }
    return "";
}

std::string dispDiffPA(std::vector<float> diffVect)
{
    std::string diffStr = "";
    int counter = 1;
    for (float &diff : diffVect)
    {
        diffStr += color::MAGENTA;
        diffStr += (counter < 10) ? "day  " : "day ";
        diffStr += std::to_string(counter) +
                   "    " + color::RESET;
        if (diff <= 0)
        {
            diffStr += color::GREEN;
            diff = std::abs(diff);
            diffStr += std::string(DOWN_ARROW);
        }
        else
        {
            diffStr += color::RED;
            diffStr += std::string(UP_ARROW);
        }
        diffStr += "  " + std::to_string(diff) + color::RESET + "\n";
        counter++;
    }
    return diffStr;
}

std::string BuildingProcessor::writeReport(const std::string &userInput, Facility f, int n, Usage usage, std::string fName)
{
    std::string result;

    result += color::WHITE + "Month: " + monthMap[n] + color::RESET + "\n";
    if (containsSubstring(userInput, "MEAN_CONSUMPTION"))
    {
        result += color::MAGENTA + "Mean Consumed Amount:    " +
                  std::to_string(f.avgUsage[n]) + color::RESET + "\n";
    }
    if (containsSubstring(userInput, "TOTAL_CONSUMPTION"))
    {
        result += color::MAGENTA + "Total Consumed Resource: " +
                  std::to_string(f.totalUsage[n]) + "\n";
    }
    if (containsSubstring(userInput, "TOTAL_COST"))
    {
        std::string path = buildingName + SLASH + fName;
        result += color::MAGENTA + "Total Cost to Pay:       " +
                  getCost(usage[n], f.avgUsage[n], path) + color::MAGENTA + +"\n";
    }
    if (containsSubstring(userInput, "PEAK_HOUR"))
    {
        result += color::MAGENTA + "peak Hour in Month:      " +
                  std::to_string(f.peakHours[n]) + color::RESET + "\n";
    }
    if (containsSubstring(userInput, "DELTA_PEAKH_AVG"))
    {
        result += dispDiffPA(f.diffPeakAvg[n]);
    }
    return result;
}

void BuildingProcessor::setDirName(const std::string &dir_name)
{
    dir_name_ = dir_name;
}

void BuildingProcessor::setBuildingName()
{
    size_t lastSlashPosition = dir_name_.find_last_of(SLASH);
    buildingName = dir_name_.substr(lastSlashPosition + 1);
    server_ = std::make_shared<NamedPipeServer>(buildingName);
}

BuildingProcessor::~BuildingProcessor()
{
    NamedPipe::remove_pipe(buildingName);
}

void BuildingProcessor::decodeData(std::string encodedString, int id)
{
    Facility f;
    EncoderDecoder::decode(encodedString, f.peakHours,
                           f.avgUsage, f.totalUsage,
                           f.diffPeakAvg);
    facilities.push_back(f);
}

std::vector<int> decodeMonths(std::string &input)
{
    if (input == std::string("all\0", 4))
        input = "1 2 3 4 5 6 7 8 9 10 11 12";

    std::vector<int> result;
    std::istringstream iss(input);
    std::string number;
    while (iss >> number)
    {
        result.push_back(stoi(number));
    }

    return result;
}

std::string BuildingProcessor::setFacilityDetail(std::string fName, Usage usage, Facility f,
                                                 std::string order, std::vector<int> months)
{
    std::string report = color::YELLOW + "---------\n" + fName + "\n---------" +
                         color::RESET + "\n";
    for (int m : months)
        report += writeReport(order, f, m, usage, fName);
    return report;
}

void BuildingProcessor::sendReportContent()
{
    std::string report = "\n";
    std::vector<int> months = decodeMonths(r.monthId);
    if (r.order == "all")
        r.order = "MEAN_CONSUMPTION TOTAL_CONSUMPTION TOTAL_COST PEAK_HOUR DELTA_PEAKH_AVG";

    report += color::YELLOW + "Report of builing " + buildingName + color::RESET + ":\n";

    if (containsSubstring(r.fName, "Gas"))
        report += setFacilityDetail("Gas", usages[0], facilities[0], r.order, months);
    if (containsSubstring(r.fName, "Water"))
        report += setFacilityDetail("Water", usages[1], facilities[1], r.order, months);
    if (containsSubstring(r.fName, "Electricity"))
        report += setFacilityDetail("Electricity", usages[2], facilities[2], r.order, months);

    report += "\n\n";
    writeFd(report.c_str(), report.size(), wFd);
    close(wFd);
    log(ss, "building process sent report to unnamed pipe", color::YELLOW);
}

void BuildingProcessor::getReportOrder()
{
    std::string msg = readFd(rFd);
    close(rFd);
    log(ss, "building process received msg from named pipe", color::YELLOW);
    EncoderDecoder::decode(msg, r.fName, r.order, r.monthId);
}

void BuildingProcessor::addToEncUsgs(std::string res, int fId, std::string path)
{
    std::string parameters, usage;
    size_t starPos = res.find('*');
    if (starPos != std::string::npos)
    {
        parameters = res.substr(0, starPos);
        usage = res.substr(starPos + 1);
    }
    decodeData(parameters, fId);
    usages.push_back(EncoderDecoder::decodeUsage(usage));
    encodedPathUsages.push_back(path + STAR + usage);
}

void BuildingProcessor::getFacilityDetails()
{
    log(ss, buildingName + " building processor successfully entered", color::YELLOW);
    std::string path;
    int facilityId = 0;
    for (const std::string &facility : facilityNames)
    {
        path = dir_name_ + SLASH + facility;

        std::vector<int> pipeFds = createPipe();

        int pid = fork();
        if (pid == 0)
        {
            close(1);
            dup2(pipeFds[1], 1);
            close(pipeFds[1]);

            close(pipeFds[0]);
            execl(FACILITY_PROCESS, FACILITY_PROCESS, path.c_str(), (char *)NULL);
        }
        else if (pid > 0)
        {
            wait(NULL);
            close(pipeFds[1]);
            std::string res = readFd(pipeFds[0]);
            close(pipeFds[0]);
            log(ss, "building process received msg from chile named pipe", color::YELLOW);
            addToEncUsgs(res, facilityId, path);
        }
        else
            throw std::runtime_error("fork() failed in running buildingProcessor.");

        facilityId++;
    }
}
