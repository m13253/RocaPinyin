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

#ifndef ROCAPINYIN_H_INCLUDED_
#define ROCAPINYIN_H_INCLUDED_
#ifdef __cplusplus

#include <cstdint>
#include <string>
#include <vector>

namespace rocapinyin {
const char *getpinyin(uint32_t ucs);
std::string getpinyin_str(const std::string &utf8str, bool strict = false);
std::vector<const char *> getpinyin_vector(const std::string &utf8str, bool strict = false);
}

extern "C" {
const char *rocapinyin_getpinyin(uint32_t ucs);
char *rocapinyin_getpinyin_str(const char *utf8str, int strict);
char *rocapinyin_getpinyin_str_free(const char *str);
const char **rocapinyin_getpinyin_vector(const char *utf8str, int strict);
const char **rocapinyin_getpinyin_vector_free(const char **vec);
}

#else

#include <stdint.h>

const char *rocapinyin_getpinyin(uint32_t ucs);
char *rocapinyin_getpinyin_str(const char *utf8str, int strict);
char *rocapinyin_getpinyin_str_free(const char *str);
const char **rocapinyin_getpinyin_vector(const char *utf8str, int strict);
const char **rocapinyin_getpinyin_vector_free(const char **vec);

#endif
#endif
