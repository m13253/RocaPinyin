#!/usr/bin/env python3

import sys
import re
import unicodedata


REGEX_PINYIN = re.compile('[a-z\u0304\u0301\u030c\u0300]+$')
REGEX_TONE = re.compile('[\u0304\u0301\u030c\u0300]')


def normalize_pinyin(pinyin):
    pinyin = unicodedata.normalize('NFD', pinyin.strip()).replace('u\u0308', 'v')
    return pinyin if REGEX_PINYIN.match(pinyin) else None


def strip_pinyin_tones(pinyin):
    pinyin_normalized = normalize_pinyin(pinyin)
    if not pinyin_normalized:
        raise ValueError('invalid pinyin: %s' % pinyin_normalized)
    return REGEX_TONE.sub('', pinyin_normalized)

def main(_, identifier, ucs_gte, ucs_lte, fn_readings, fn_variants):
    ucs_gte, ucs_lte = int(ucs_gte, 16), int(ucs_lte, 16)
    sys.stdout.write('static const struct rocapinyin::char_data_t %s = {\n' % identifier)
    pinyin_table = {}
    with open(fn_readings, "r") as file_readings:
        for line in file_readings:
            line = line.split('\t')
            if not line[0].startswith('U+'):
                continue
            ucs = int(line[0][2:], 16)
            if line[1] == 'kMandarin':
                pinyin_table[ucs] = strip_pinyin_tones(line[2])
    pinyin_table_subset = {key: value for key, value in pinyin_table.items() if ucs_gte <= key <= ucs_lte}
    if pinyin_table_subset:
        ucs_gte = min(pinyin_table_subset)
        ucs_lt = max(pinyin_table_subset)+1
    else:
        ucs_lt = ucs_gte
    sys.stdout.write('    0x%x, 0x%x,\n    (const char *[]) {\n' % (ucs_gte, ucs_lt))
    for idx in range(ucs_gte, ucs_lt):
        if idx in pinyin_table_subset:
            sys.stdout.write('        "%s",\n' % pinyin_table_subset[idx])
        else:
            sys.stdout.write('        nullptr,\n')
    sys.stdout.write('    }\n};\n')

if __name__ == '__main__':
    sys.exit(main(*sys.argv))
