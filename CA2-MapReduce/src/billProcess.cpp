#include "billProcessor.hpp"

int main(int argc, char const *argv[])
{
    std::stringstream ss;
    log(ss, "bill process successfully created", color::MAGENTA);

    if (argc < 1)
    {
        log(ss, "missing required argumants", color::RED);
        exit(EXIT_FAILURE);
    }
    try
    {
        BillProcessor billProcessor;
        billProcessor.setDirName(argv[1]);
        billProcessor.run();
    }
    catch (const std::exception &e)
    {
        log(ss, e.what(), color::RED);
        exit(EXIT_FAILURE);
    }

    log(ss, "exiting successfully from bill process.", color::MAGENTA);

    return EXIT_SUCCESS;
}
