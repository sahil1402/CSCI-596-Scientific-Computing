#include "harbortune/version.hpp"

#include <iostream>
#include <string>

namespace {

void print_help() {
    std::cout
        << "HarborTune - parallel screening of breakwater entrances\n\n"
        << "Usage:\n"
        << "  harbortune help\n"
        << "  harbortune --version\n\n"
        << "Simulation, sweep, verification, and benchmark commands will be "
           "added as the implementation progresses.\n";
}

}  // namespace

int main(int argc, char** argv) {
    if (argc == 1 || std::string(argv[1]) == "help" ||
        std::string(argv[1]) == "--help") {
        print_help();
        return 0;
    }

    if (std::string(argv[1]) == "--version") {
        std::cout << "HarborTune " << HARBORTUNE_VERSION << '\n';
        return 0;
    }

    std::cerr << "Unknown command: " << argv[1] << "\n\n";
    print_help();
    return 2;
}

