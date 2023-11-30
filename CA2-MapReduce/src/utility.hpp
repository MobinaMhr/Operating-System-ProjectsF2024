#include <vector>
#include <string>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdexcept>
#include <string>
#include <vector>
#include <ostream>
#include <iostream>
#include <fstream>
#include <sstream>
#include <numeric>
#include <algorithm>

#include "consts.hpp"

std::vector<std::string> split(const std::string &str);

CsvData readCsv(const std::string &filename);

Usage extractData(CsvData readData);

bool containsSubstring(const std::string &input, const std::string &substring);

int argMax(std::vector<float> vect);

void log(std::stringstream &ss, const std::string &message, std::string c);

void displayHelpPrompt();

void log_(std::stringstream &ss, const std::string &message, std::string c);