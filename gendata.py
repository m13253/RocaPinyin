#!/usr/bin/env python3

# Copyright (C) 2014  StarBrilliant <m13253@hotmail.com>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 3.0 of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this program.  If not,
# see <http://www.gnu.org/licenses/>.

import logging
import re
import sys
import unicodedata


if sys.version_info < (3,):
    sys.stderr.write('This script requires Python 3 or higher version.\n')
    sys.exit(1)


REGEX_PINYIN = re.compile('[a-z\u0304\u0301\u030c\u0300]+')
REGEX_TONE = re.compile('[\u0304\u0301\u030c\u0300]')
REGEX_UCS = re.compile('\\bU\\+[0-9A-F]+\\b')
if sys.version_info < (3, 4):
    REGEX_PINYIN_FULLMATCH = re.compile('[a-z\u0304\u0301\u030c\u0300]+$')


def find_pinyin(pinyin):
    pinyin = unicodedata.normalize('NFD', pinyin.strip()).replace('u\u0308', 'v').replace('e\u0302', 'eh')
    pinyin_match = REGEX_PINYIN.search(pinyin)
    if not pinyin_match:
        raise ValueError('invalid pinyin: %s' % pinyin)
    return pinyin_match.group(0)


def normalize_pinyin(pinyin):
    pinyin = unicodedata.normalize('NFD', pinyin.strip()).replace('u\u0308', 'v').replace('e\u0302', 'eh')
    if sys.version_info >= (3, 4):
        pinyin_match = REGEX_PINYIN.fullmatch(pinyin)
    else:
        pinyin_match = REGEX_PINYIN_FULLMATCH.match(pinyin)
    if not pinyin_match:
        raise ValueError('invalid pinyin: %s' % pinyin)
    return pinyin_match.group(0)


def strip_pinyin_tones(pinyin):
    if sys.version_info >= (3, 4):
        pinyin_match = REGEX_PINYIN.fullmatch(pinyin)
    else:
        pinyin_match = REGEX_PINYIN_FULLMATCH.match(pinyin)
    if not pinyin_match:
        raise ValueError('invalid pinyin: %s' % pinyin)
    return REGEX_TONE.sub('', pinyin)


def parse_variant(desc):
    for i in REGEX_UCS.finditer(desc):
        yield int(i.group(0)[2:], 16)


def push_result(result, previous, file_out):
    if result is not None:
        width = 4+sum(map(len, previous))+len(previous)*2+len(result)
        if width >= 78:
            file_out.write(' '*4)
            file_out.write(', '.join(previous))
            file_out.write(',\n')
            del previous[:]
        previous.append(result)
    else:
        if previous:
            file_out.write(' '*4)
            file_out.write(', '.join(previous))
            file_out.write('\n')
            del previous[:]


def main(_, identifier, ucs_gte, ucs_lte, fn_readings, fn_variants):
    logging.basicConfig(format='%(message)s', level=logging.INFO)

    ucs_gte, ucs_lte = int(ucs_gte, 16), int(ucs_lte, 16)
    sys.stdout.write('''/*
  Copyright (C) 1991-2014 Unicode, Inc. All rights reserved.
  Distributed under the Terms of Use in 
  http://www.unicode.org/copyright.html .

  Data converted by StarBrilliant.
  Copyright (C) 2014 StarBrilliant <m13253@hotmail.com>
*/

''')

    pinyin_table = {}
    variant_table = {}

    logging.info('Stage 1: reading explicit pinyin data from Unihan_Readings.txt')
    with open(fn_readings, "r", encoding="utf-8") as file_readings:
        for line in file_readings:
            line = line.split('\t', 2)
            if not line[0].startswith('U+'):
                continue
            ucs = int(line[0][2:], 16)
            if line[1] == 'kHanyuPinyin':
                if ucs not in pinyin_table:
                    pinyin_table[ucs] = (4,)
                pinyin_table[ucs] = min(pinyin_table[ucs], (1, strip_pinyin_tones(find_pinyin(line[2]))))
            elif line[1] == 'kMandarin':
                if ucs not in pinyin_table:
                    pinyin_table[ucs] = (4,)
                pinyin_table[ucs] = min(pinyin_table[ucs], (0, strip_pinyin_tones(normalize_pinyin(line[2]))))
            elif line[1] == 'kXHC1983':
                if ucs not in pinyin_table:
                    pinyin_table[ucs] = (4,)
                pinyin_table[ucs] = min(pinyin_table[ucs], (2, strip_pinyin_tones(find_pinyin(line[2]))))
            elif line[1] == 'kDefinition':
                if ucs not in variant_table:
                    variant_table[ucs] = [[], [], [], [], [], []]
                variant_table[ucs][5] = list(parse_variant(line[2]))

    logging.info('Stage 2: reading character variation data from Unihan_Variants.txt')
    with open(fn_variants, "r", encoding="utf-8") as file_variants:
        for line in file_variants:
            line = line.split('\t', 2)
            if not line[0].startswith('U+'):
                continue
            ucs = int(line[0][2:], 16)
            if ucs not in variant_table:
                variant_table[ucs] = [[], [], [], [], [], []]
            index = {'kZVariant': 0, 'kSimplifiedVariant': 1, 'kTraditionalVariant': 2, 'kSemanticVariant': 3, 'kSpecializedSemanticVariant': 4, 'kDefinition': 5}.get(line[1])
            if index is not None:
                variant_table[ucs][index] = list(parse_variant(line[2]))

    variant_entry_maxlen = 0
    for variant_entry in variant_table.values():
        variant_entry_maxlen = max(variant_entry_maxlen, max(map(len, variant_entry)))

    stage_count = 2
    flag1 = True
    while flag1:
        flag1 = False
        stage_count += 1
        logging.info('Stage %s: inferring pinyin among character variants (forward direction)' % stage_count)
        for variant_type in range(6):
            flag2 = True
            while flag2:
                flag2 = False
                for variant_index in range(variant_entry_maxlen):
                    pinyin_variant_unavailable = [key for key in variant_table.keys() if key not in pinyin_table]
                    pinyin_variant_unavailable.sort()
                    for ucs in pinyin_variant_unavailable:
                        try:
                            variant_entry = variant_table[ucs][variant_type][variant_index]
                        except IndexError:
                            continue
                        if variant_entry in pinyin_table:
                            pinyin_table[ucs] = (3, pinyin_table[variant_entry][1])
                            flag1, flag2 = True, True
        stage_count += 1
        logging.info('Stage %s: inferring pinyin among character variants (reversed direction)' % stage_count)
        for variant_type in range(6):
            flag2 = True
            while flag2:
                flag2 = False
                for variant_index in range(variant_entry_maxlen):
                    pinyin_variant_available = [key for key in variant_table.keys() if key in pinyin_table]
                    pinyin_variant_available.sort()
                    for ucs in pinyin_variant_available:
                        try:
                            variant_entry = variant_table[ucs][variant_type][variant_index]
                        except IndexError:
                            continue
                        if variant_entry not in pinyin_table:
                            pinyin_table[variant_entry] = (3, pinyin_table[ucs][1])
                            flag1, flag2 = True, True

    stage_count += 1
    logging.info('Stage %s: inferring pinyin among Unicode identical characters' % stage_count)
    for ucs in range(ucs_gte, ucs_lte+1):
        if ucs not in pinyin_table or pinyin_table[ucs][0] >= 3:
            ucs_nfkc = ord(unicodedata.normalize('NFKC', chr(ucs)))
            if ucs_nfkc in pinyin_table and pinyin_table[ucs_nfkc][0] < 3:
                pinyin_table[ucs] = pinyin_table[ucs_nfkc]

    stage_count += 1
    logging.info('Stage %s: generating C++ source code' % stage_count)
    pinyin_table_subset = {ucs for ucs in pinyin_table.keys() if ucs_gte <= ucs <= ucs_lte}
    if pinyin_table_subset:
        ucs_gte = min(pinyin_table_subset)
        ucs_lt = max(pinyin_table_subset)+1
    else:
        ucs_lt = ucs_gte

    sys.stdout.write('static const char *const %s_pinyin[] = {\n' % identifier)
    output_previous = []
    for ucs in range(ucs_gte, ucs_lt):
        if ucs in pinyin_table:
            push_result('"%s"' % pinyin_table[ucs][1], output_previous, sys.stdout)
        else:
            push_result('nullptr', output_previous, sys.stdout)
    push_result(None, output_previous, sys.stdout)
    assert not output_previous
    sys.stdout.write('};\nstatic const struct rocapinyin::char_data_t %s = {\n    0x%x, 0x%x, %s_pinyin\n};\n' % (identifier, ucs_gte, ucs_lt, identifier))

if __name__ == '__main__':
    sys.exit(main(*sys.argv))
