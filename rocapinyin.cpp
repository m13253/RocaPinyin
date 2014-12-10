#include <cstddef>
#include <iostream>
#include <string>

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

std::string getpinyin(const std::string &s) {
    return std::string();
}

}

int main() {
    return 0;
}