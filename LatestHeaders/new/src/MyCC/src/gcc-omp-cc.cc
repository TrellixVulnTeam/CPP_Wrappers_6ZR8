#include "./include/CPPFileIO.hh"

int main(int const argc, char const **argv) {
    std::vector<std::string> args;
    args.push_back("/usr/bin/gcc");
    for (int i = 1; i < argc; i++) { args.push_back(argv[i]); }
    args.push_back("-Ofast");
    args.push_back("-mtune=native");
    args.push_back("-march=native");
    args.push_back("-fopenmp");
    CPPFileIO::starter_self(args);
}
