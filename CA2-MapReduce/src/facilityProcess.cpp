#include "facilityProcessor.hpp"

int main(int argc, char const *argv[])
{
    std::stringstream ss;
    log(ss, "facility process successfully created", color::GREEN);

    if (argc < 1)
    {
        log(ss, "missing some arguments", color::RED);
        exit(EXIT_FAILURE);
    }

    try
    {
        close(0);
        FacilityProcessor facilityProcssr;
        facilityProcssr.setDirName(argv[1]);
        std::string path = "./" + std::string(argv[1]) + ".csv";
        std::string toSend = facilityProcssr.run(path);
        toSend.push_back('\0'); // Add null terminator
        writeFd(toSend.c_str(), toSend.size(), 1);
        close(1);
        log(ss, "facility processor successfully sent extracted data.", color::GREEN);
    }
    catch (const std::exception &e)
    {
        log(ss, e.what(), color::RED);
        exit(EXIT_FAILURE);
    }

    log(ss, "facility process successfully exited", color::GREEN);
    return EXIT_SUCCESS;
}