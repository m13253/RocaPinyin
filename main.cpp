/*
  Copyright (C) 2014  StarBrilliant <m13253@hotmail.com>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 3.0 of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this program.  If not,
  see <http://www.gnu.org/licenses/>.
*/

#include <cstdio>
#include <iostream>
#include <string>
#include <unistd.h>
#include "rocapinyin.h"

int main() {
    bool stdin_isatty = !!isatty(fileno(stdin));
    while(std::cin) {
        if(stdin_isatty)
            std::cerr << "> " << std::flush;
        std::string line;
        if(std::getline(std::cin, line))
            std::cout << rocapinyin::getpinyin_str(line) << std::endl;
    }
    return 0;
}
