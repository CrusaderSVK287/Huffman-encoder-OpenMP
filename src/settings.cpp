#include "settings.hpp"
#include <omp.h>
#include <stdexcept>
#include <string>

Settings::Settings(int argc, char **argv)
{
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-n")) {
            this->threadCount = std::stoi(argv[i+1]);
            i++;
        }
        else if (!strcmp(argv[i], "-d") || !strcmp(argv[i], "--decode")) {decode = true;}
        else if (!strcmp(argv[i], "--debug")) {debug = true;}
        else if (path.empty()) {
            path = argv[i];
        } else {
            throw std::invalid_argument("Invalid argument");
        }
    }
}
    
void Settings::check() 
{
    if (threadCount <= 0) 
        throw std::invalid_argument("Wrong -n config");
    if (threadCount > omp_get_num_procs()) 
        throw std::invalid_argument("Maximum amount of threads can be " + std::to_string(omp_get_num_procs()));
    if (path == "") 
        throw std::invalid_argument("Wrong path");
}
