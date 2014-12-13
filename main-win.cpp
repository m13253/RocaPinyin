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

#ifndef _WIN32
#error main-win.cpp is patched for Microsoft Windows with buggy Unicode I/O support, use main.cpp for other OSes.
#endif

#include <cstdio>
#include <iostream>
#include <string>
#include <windows.h>
#include <libwintf8.h> /* https://github.com/m13253/libWinTF8 */
#include "rocapinyin.h"

int main() {
    WTF8::SetConsoleFont _;
    DWORD dummy;
    bool stdin_isatty = !!GetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), &dummy);
    while(WTF8::cin) {
        if(stdin_isatty)
            WTF8::cerr << "> " << std::flush;
        WTF8::u8string line;
        if(std::getline(WTF8::cin, line)) {
            if(line == WTF8::u8string("\x04\r") || line == WTF8::u8string("\x1a\r"))
                break;
            WTF8::cout << rocapinyin::getpinyin(line) << std::endl;
        }
    }
    return 0;
}
