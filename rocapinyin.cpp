#include <cstddef>
#include <cstdio>
#include <initializer_list>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unistd.h>

namespace rocapinyin {

struct char_data_t {
    uint32_t ucs_gte;
    uint32_t ucs_lt;
    const char **pinyin;
};

#include "cache/char_cjk_main.h"
#include "cache/char_cjk_exta.h"
#include "cache/char_cjk_extb.h"
#include "cache/char_cjk_extc.h"
#include "cache/char_cjk_extd.h"

const char *getpinyin(uint32_t ucs) {
    for(const struct char_data_t &block : {
        char_cjk_main, char_cjk_exta, char_cjk_extb, char_cjk_extc, char_cjk_extd
    })
        if(ucs >= block.ucs_gte && ucs < block.ucs_lt)
            return block.pinyin[ucs-block.ucs_gte];
    return nullptr;
}

static bool utf8_check_continuation(const std::string &utf8str, size_t start, size_t check_length) {
    if(utf8str.size() > start + check_length) {
        while(check_length--)
            if((uint8_t(utf8str[++start]) & 0xc0) != 0x80)
                return false;
        return true;
    } else
        return false;
}

std::string getpinyin(const std::string &utf8str, bool strict = false) {
    size_t i = 0;
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
    result.shrink_to_fit();
    return result;
}

}

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