// Testing file provided for EECS 281 Lab 7
// Project Identifier: 2C4A3C53CD5AD45A7BEA3AE5356A3B1622C9D04B
#include "hashtable.h"
#include <string>
#include <iostream>

int main() {
    // VERY BASIC EXAMPLE (will likely not compile with default/empty hashtable.h file)
    HashTable<std::string, int> midterm;
    // std::cout << midterm.insert("sam", 50) << " ";
    // std::cout << midterm.insert("fee", 100) << " ";
    // std::cout << midterm.insert("milo", 95) << " ";
    // std::cout << midterm.insert("gabe", 88) << " \n";
    std::cout << midterm.insert("1", 50) << " ";
    std::cout << midterm.insert("11", 100) << " ";
    std::cout << midterm.insert("110", 95) << " ";
    std::cout << midterm.insert("111", 95) << " ";
    std::cout << midterm.insert("109", 95) << " ";
    std::cout << midterm.insert("1111", 95) << " ";
    std::cout << midterm.insert("21", 95) << " ";
    // std::cout << midterm.insert("3", 95) << " ";
    // std::cout << midterm.insert("3", 95) << " ";
    // std::cout << midterm.insert("3", 95) << " ";
    // std::cout << midterm.insert("3", 95) << " ";
    // std::cout << midterm.insert("3", 95) << " ";
    // std::cout << midterm.insert("3", 95) << " ";
    // std::cout << midterm.insert("4", 88) << " \n";
    // midterm.erase("sam");
    // midterm.erase("fee");
    // midterm.erase("milo");
    // midterm.erase("gabe");
    // midterm.erase("gabe");
    // midterm.insert("takeo",22);
    // std::cout << midterm["sam"] << " ";
    // std::cout << midterm["fee"] << " ";
    // std::cout << midterm["milo"] << " ";
    // std::cout << midterm["gabe"]; 
    // std::cout << midterm["takeo"] << " \n";
    // std::cout << midterm.erase("sam") << " ";
    // std::cout << midterm["sam"] << "\n";
    // ADD MORE TESTS OF YOUR OWN
    return 0;
}
