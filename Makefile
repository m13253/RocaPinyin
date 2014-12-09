.PHONY: all clean distclean

all: cache/char_cjk_basic.h cache/char_cjk_exta.h cache/char_cjk_extb.h cache/char_cjk_extc.h cache/char_cjk_extd.h

Unihan.zip:
	wget -c -O Unihan.zip.part http://www.unicode.org/Public/UCD/latest/ucd/Unihan.zip
	mv Unihan.zip.part Unihan.zip

cache:
	mkdir cache

cache/Unihan_Readings.txt cache/Unihan_Variants.txt: Unihan.zip cache
	unzip -o -d cache $< ${subst cache/,,$@}

cache/char_cjk_basic.h: gendata.py cache/Unihan_Readings.txt cache/Unihan_Variants.txt
	./gendata.py char_cjk_basic 4E00 9FFF cache/Unihan_Readings.txt cache/Unihan_Variants.txt > $@

cache/char_cjk_exta.h: gendata.py cache/Unihan_Readings.txt cache/Unihan_Variants.txt
	./gendata.py char_cjk_exta 3400 4DBF cache/Unihan_Readings.txt cache/Unihan_Variants.txt > $@

cache/char_cjk_extb.h: gendata.py cache/Unihan_Readings.txt cache/Unihan_Variants.txt
	./gendata.py char_cjk_extb 20000 2A6DF cache/Unihan_Readings.txt cache/Unihan_Variants.txt > $@

cache/char_cjk_extc.h: gendata.py cache/Unihan_Readings.txt cache/Unihan_Variants.txt
	./gendata.py char_cjk_extb 2A700 2B73F cache/Unihan_Readings.txt cache/Unihan_Variants.txt > $@

cache/char_cjk_extd.h: gendata.py cache/Unihan_Readings.txt cache/Unihan_Variants.txt
	./gendata.py char_cjk_extb 2B740 2B81F cache/Unihan_Readings.txt cache/Unihan_Variants.txt > $@
