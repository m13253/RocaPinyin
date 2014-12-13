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
