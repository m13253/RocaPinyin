#include <cstdio>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include "rocapinyin.h"

int main() {
    while(std::cin) {
        if(isatty(fileno(stdin)))
            std::cerr << "> " << std::flush;
        std::stringstream linebuf;
        if(std::cin.get(*linebuf.rdbuf())) {
            std::cout << rocapinyin::getpinyin(linebuf.str()) << std::endl;
        }
        char c;
        std::cin.get(c); /* bypass \n */
    }
    return 0;
}
