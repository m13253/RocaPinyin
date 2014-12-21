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

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <functional>
#include <initializer_list>
#include <stdexcept>
#include <string>
#include <vector>

namespace rocapinyin {

struct char_data_t {
    uint32_t ucs_gte;
    uint32_t ucs_lt;
    const char *const *pinyin;
};

#include "cache/char_cjk_main.h"
#include "cache/char_cjk_exta.h"
#include "cache/char_cjk_extb.h"
#include "cache/char_cjk_extc.h"
#include "cache/char_cjk_extd.h"
#include "cache/char_cjk_exte.h"
#include "cache/char_cjk_comp.h"
#include "cache/char_cjk_compsup.h"

const char *getpinyin(uint32_t ucs) {
    switch(ucs) {
    case 0xb7:
    case 0x30fb:
        return "\xC2\xB7";
    case 0x2014:
    case 0x30fc:
        return "-";
    case 0x2018:
    case 0x3008:
    case 0x300c:
        return "\xE2\x80\x98";
    case 0x2019:
    case 0x3009:
    case 0x300d:
        return "\xE2\x80\x99";
    case 0x201c:
    case 0x300e:
    case 0x300a:
        return "\xE2\x80\x9C";
    case 0x201d:
    case 0x300f:
    case 0x300b:
        return "\xE2\x80\x9D";
    case 0x3000:
        return " ";
    case 0x3001:
        return ",";
    case 0x3002:
        return ".";
    case 0x3007:
        return "ling";
    case 0x3010:
    case 0x3014:
    case 0x3016:
        return "[";
    case 0x3011:
    case 0x3015:
    case 0x3017:
        return "]";
    case 0x301c:
        return "~";
    }
    const auto convert_halfwidth = [](uint32_t ucs) -> const char * {
        static char halfwidth_cache[192] = { 0 };
        if(!halfwidth_cache[(ucs-0xff00)*2])
            halfwidth_cache[(ucs-0xff00)*2] = ucs-0xfee0;
        return halfwidth_cache+(ucs-0xff00)*2;
    };
    if(ucs > 0xff00 && ucs < 0xff5f)
        return convert_halfwidth(ucs);
    for(const struct char_data_t &block : {
        char_cjk_main, char_cjk_exta, char_cjk_extb, char_cjk_extc, char_cjk_extd, char_cjk_exte, char_cjk_comp, char_cjk_compsup
    })
        if(ucs >= block.ucs_gte && ucs < block.ucs_lt)
            return block.pinyin[ucs-block.ucs_gte];
    return nullptr;
}

static void parse_utf8_string(const std::string &utf8str, std::function<void(uint32_t ucs, std::initializer_list<char> fallback)> append_result, bool strict) {
    const auto utf8_check_continuation = [](const std::string &utf8str, size_t start, size_t check_length) -> bool {
        if(utf8str.size() > start + check_length) {
            while(check_length--)
                if((uint8_t(utf8str[++start]) & 0xc0) != 0x80)
                    return false;
            return true;
        } else
            return false;
    };
    size_t i = 0;
    while(i < utf8str.size()) {
        if(uint8_t(utf8str[i]) < 0x80) {
            uint32_t ucs4 = utf8str[i];
            append_result(ucs4, {utf8str[i]});
            ++i;
            continue;
        } else if(uint8_t(utf8str[i]) < 0xc0) {
        } else if(uint8_t(utf8str[i]) < 0xe0) {
            if(utf8_check_continuation(utf8str, i, 1)) {
                uint32_t ucs4 = uint32_t(utf8str[i] & 0x1f) << 6 | uint32_t(utf8str[i+1] & 0x3f);
                if(ucs4 >= 0x80) {
                    append_result(ucs4, {utf8str[i], utf8str[i+1]});
                    i += 2;
                    continue;
                }
            }
        } else if(uint8_t(utf8str[i]) < 0xf0) {
            if(utf8_check_continuation(utf8str, i, 2)) {
                uint32_t ucs4 = uint32_t(utf8str[i] & 0xf) << 12 | uint32_t(utf8str[i+1] & 0x3f) << 6 | (utf8str[i+2] & 0x3f);
                if(ucs4 >= 0x800 && (ucs4 & 0xf800) != 0xd800) {
                    append_result(ucs4, {utf8str[i], utf8str[i+1], utf8str[i+2]});
                    i += 3;
                    continue;
                }
            }
        } else if(uint8_t(utf8str[i]) < 0xf8) {
            if(utf8_check_continuation(utf8str, i, 3)) {
                uint32_t ucs4 = uint32_t(utf8str[i] & 0x7) << 18 | uint32_t(utf8str[i+1] & 0x3f) << 12 | uint32_t(utf8str[i+2] & 0x3f) << 6 | uint32_t(utf8str[i+3] & 0x3f);
                if(ucs4 >= 0x10000 && ucs4 < 0x110000) {
                    append_result(ucs4, {utf8str[i], utf8str[i+1], utf8str[i+2], utf8str[i+3]});
                    i += 4;
                    continue;
                }
            }
        }
        if(strict)
            throw std::runtime_error("invalid UTF-8 text");
        else {
            append_result(0xfffd, {'\xef', '\xbf', '\xbd'});
            ++i;
        }
    }
}

std::string getpinyin_str(const std::string &utf8str, bool strict = false) {
    std::string result;
    result.reserve(utf8str.length()*3);
    enum {
        RS_WHITESPACE,
        RS_TRANSLITERATE,
        RS_TRANSCRIPTION
    } result_state = RS_WHITESPACE;
    const auto append_result = [&](uint32_t ucs, std::initializer_list<char> fallback) {
        if(ucs == ' ' || ucs == '\n' || ucs == '\r' || ucs == '\t' || ucs == '\0') {
            result.append(fallback);
            result_state = RS_WHITESPACE;
        } else {
            const char *syllable = getpinyin(ucs);
            if(syllable != nullptr) {
                if(result_state != RS_WHITESPACE)
                    result += ' ';
                result += syllable;
                result_state = RS_TRANSLITERATE;
            } else {
                if(result_state == RS_TRANSLITERATE)
                    result += ' ';
                result.append(fallback);
                result_state = RS_TRANSCRIPTION;
            }
        }
    };
    parse_utf8_string(utf8str, append_result, strict);
    result.shrink_to_fit();
    return result;
}

std::vector<const char *> getpinyin_vector(const std::string &utf8str, bool strict = false) {
    std::vector<const char *> result;
    result.reserve(utf8str.length()/2);
    const auto append_result = [&](uint32_t ucs, std::initializer_list<char> fallback) {
        result.push_back(getpinyin(ucs));
        static_cast<void>(fallback);
    };
    parse_utf8_string(utf8str, append_result, strict);
    result.shrink_to_fit();
    return result;
}

}

extern "C" {

const char *rocapinyin_getpinyin(uint32_t ucs) {
    return rocapinyin::getpinyin(ucs);
}

char *rocapinyin_getpinyin_str(const char *utf8str, int strict) {
    std::string cppstr = rocapinyin::getpinyin_str(utf8str, strict != 0);
    char *cstr = new char[cppstr.length()+1];
    memcpy(cstr, cppstr.data(), cppstr.length()*sizeof (char));
    cstr[cppstr.length()] = '\0';
    return cstr;
}

char *rocapinyin_getpinyin_str_free(const char *str) {
    if(str)
        delete[] str;
    return nullptr;
}

const char **rocapinyin_getpinyin_vector(const char *utf8str, int strict) {
    std::vector<const char *> cppvec = rocapinyin::getpinyin_vector(utf8str, strict != 0);
    const char **cvec = new const char *[cppvec.size()+1];
    memcpy(cvec, cppvec.data(), cppvec.size()*sizeof (char *));
    cvec[cppvec.size()] = nullptr;
    return cvec;
}

const char **rocapinyin_getpinyin_vector_free(const char **vec) {
    if(vec)
        delete[] vec;
    return nullptr;
}

}
