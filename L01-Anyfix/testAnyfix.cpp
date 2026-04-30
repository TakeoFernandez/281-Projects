#include <iostream>
#include <string>

#include "evaluate.hpp"


auto main() -> int {
    // Do whatever you want here to test your solution.
    std::cout << evaluate("+12") << '\n';   //3
    std::cout << evaluate("3*4") << '\n';   //12
    std::cout << evaluate("56-") << '\n';   //-1
    std::cout << evaluate("1-2*3") << '\n'; //-3
    std::cout << evaluate("321-/") << '\n'; //3/(2-1) = 3
    std::cout << evaluate("*+456") << '\n'; //(4+5) * 6 = 54
    std::cout << evaluate("/+453") << '\n'; //(4+5) * 6 = 54
    std::cout << evaluate("123+4+-") << '\n'; //8
    std::cout << evaluate("4/5") << '\n'; //0
    std::cout << evaluate("1234567+-/+**") << '\n'; //0

}
/*
 * NOTE: Since this file's name matches the glob pattern test*.cpp, the file is
 * a test file, NOT a source file! Nothing written here will be
 * submitted to the autograder.
 *
 * This also means there is no executable for this lab. Do not set EXECUTABLE
 * in your makefile to anything intended to be meaningful. Do not build with
 * `make release` or `make debug` or `make all` or just plain `make`; instead,
 * run `make help` and read the output that gives you, then use what you learn
 * from that to figure out how to build a test executable using this file.
 */
