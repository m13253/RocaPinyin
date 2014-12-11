#include <cstddef>
#include <string>

namespace rocapinyin {
const char *getpinyin(uint32_t ucs);
std::string getpinyin(const std::string &utf8str, bool strict = false);
}
