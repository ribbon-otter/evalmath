//This file is licensed under MIT-0
//Copyright 2026 Ribbon-otter

//This is a small commandline calculator; used by fuzz.pl to test the library
#include "evalmath.h"
#include <print>

using std::println;

int main(int argc, char** argv) {
    if (argc != 2) println(stderr, "call with exactly one argument");
    std::string formula = argv[1];
    auto result = evalmath::eval(formula);
    if (result) {
        println("{}", *result);
        return 0;
    } else {
        println(stderr, "error: {}", result.error());
        return 1;
    }
}
// vim: ts=4 sw=4 expandtab
