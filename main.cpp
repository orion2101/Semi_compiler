#include <iostream>
#include "modules/Compile.h"

/**
 * to build call: g++ main.cpp task_01.cpp -o task
 * or make
 */


int main() {
    std::cout << "Ah, here we go again" << std::endl;
    Compile("D:\\pascalTest.p");
//    Parse2("D:\\test.p");

    return 0;
}
