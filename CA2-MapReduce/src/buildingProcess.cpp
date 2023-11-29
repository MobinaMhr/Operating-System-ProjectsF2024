#include "buildingProcessor.hpp"

int main(int argc, char const *argv[])
{
    std::stringstream ss;
    log(ss, "building process successfully created", color::YELLOW);

    if (argc < 4)
    {
        log(ss, "missing required arguments", color::RED);
        exit(EXIT_FAILURE);
    }
    try
    {
        BuildingProcessor building;
        building.setDirName(argv[1]);
        building.setFds(argv[2], argv[3]);
        building.setBuildingName();
        building.getFacilityDetails();
        building.getReportOrder();
        building.sendReportContent();
    }
    catch (const std::exception &e)
    {
        log(ss, e.what(), color::RED);
        exit(EXIT_FAILURE);
    }

    log(ss, "building process successfully exiting", color::YELLOW);
    return EXIT_SUCCESS;
}
