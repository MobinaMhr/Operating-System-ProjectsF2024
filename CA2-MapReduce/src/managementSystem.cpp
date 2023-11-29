#include "manager.hpp"

int main(int argc, char const *argv[])
{
    std::stringstream ss;
    if (argc < 1)
    {
        log(ss, "Buildings directory name is missing", color::RED);
        exit(EXIT_FAILURE);
    }

    try
    {
        ManagementSystem mngmntSys;
        mngmntSys.setDirName(argv[1]);
        mngmntSys.run();
    }
    catch (const std::exception &e)
    {
        log(ss, e.what(), color::RED);
        exit(EXIT_FAILURE);
    }

    log(ss, "Management System successfully exited", color::BLACK);
    return EXIT_SUCCESS;
}
