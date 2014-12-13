.PHONY: all clean distclean

AR=ar
CXX=c++
LD=$(CXX)
override CXXFLAGS:=-std=c++11 -Wall -Wextra -O3 -flto -fuse-linker-plugin $(CXXFLAGS)

all: rocapinyin librocapinyin.a

clean: distclean
	rm -rf cache

distclean:
	rm -f rocapinyin librocapinyin.a rocapinyin.o main.o cache/Unihan_Readings.txt cache/Unihan_Variants.txt

Unihan.zip:
	wget -c -O Unihan.zip.part http://www.unicode.org/Public/UCD/latest/ucd/Unihan.zip
	mv Unihan.zip.part Unihan.zip

cache/cache:
	mkdir -p cache
	touch cache/cache

cache/Unihan_Readings.txt cache/Unihan_Variants.txt: Unihan.zip cache/cache
	unzip -p $< $(subst cache/,,$@) > $@

cache/char_cjk_main.h: gendata.py cache/Unihan_Readings.txt cache/Unihan_Variants.txt
	./gendata.py char_cjk_main 4E00 9FFF cache/Unihan_Readings.txt cache/Unihan_Variants.txt > $@

cache/char_cjk_exta.h: gendata.py cache/Unihan_Readings.txt cache/Unihan_Variants.txt
	./gendata.py char_cjk_exta 3400 4DBF cache/Unihan_Readings.txt cache/Unihan_Variants.txt > $@

cache/char_cjk_extb.h: gendata.py cache/Unihan_Readings.txt cache/Unihan_Variants.txt
	./gendata.py char_cjk_extb 20000 2A6DF cache/Unihan_Readings.txt cache/Unihan_Variants.txt > $@

cache/char_cjk_extc.h: gendata.py cache/Unihan_Readings.txt cache/Unihan_Variants.txt
	./gendata.py char_cjk_extc 2A700 2B73F cache/Unihan_Readings.txt cache/Unihan_Variants.txt > $@

cache/char_cjk_extd.h: gendata.py cache/Unihan_Readings.txt cache/Unihan_Variants.txt
	./gendata.py char_cjk_extd 2B740 2B81F cache/Unihan_Readings.txt cache/Unihan_Variants.txt > $@

cache/char_cjk_exte.h: gendata.py cache/Unihan_Readings.txt cache/Unihan_Variants.txt
	./gendata.py char_cjk_exte 2B820 2CEAF cache/Unihan_Readings.txt cache/Unihan_Variants.txt > $@

cache/char_cjk_comp.h: gendata.py cache/Unihan_Readings.txt cache/Unihan_Variants.txt
	./gendata.py char_cjk_comp F900 FAFF cache/Unihan_Readings.txt cache/Unihan_Variants.txt > $@

cache/char_cjk_compsup.h: gendata.py cache/Unihan_Readings.txt cache/Unihan_Variants.txt
	./gendata.py char_cjk_compsup 2F800 2FA1F cache/Unihan_Readings.txt cache/Unihan_Variants.txt > $@

rocapinyin: main.o rocapinyin.o
	$(LD) $(CXXFLAGS) $(LDFLAGS) -o $@ $^ $(LIB)

librocapinyin.a: rocapinyin.o
	$(AR) rcs $@ $^

main.o: main.cpp rocapinyin.h

rocapinyin.o: rocapinyin.cpp cache/char_cjk_main.h cache/char_cjk_exta.h cache/char_cjk_extb.h cache/char_cjk_extc.h cache/char_cjk_extd.h cache/char_cjk_exte.h cache/char_cjk_comp.h cache/char_cjk_compsup.h

