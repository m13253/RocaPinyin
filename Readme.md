RocaPinyin
==========

A simple Hànzì (Chinese characters) to Hànyǔ Pīnyīn (Chinese reading notations) conversion library, a part of the Rocaloid project.

This library does not support tones or multi-pronunciation characters, since Rocaloid does not need them. You can modify this program to suit your needs.

Build
-----

Download [Unihan.zip](http://www.unicode.org/Public/UCD/latest/ucd/Unihan.zip) and execute `make`.

You will get an executable named `rocapinyin` and a static library named `librocapinyin.a`.

GCC option `-O3 -flto` is specified and should also be set when linking against `librocapinyin.a`

For Windows users with buggy Unicode I/O support, use `main-win.cpp` instead `main.cpp` and link against [libWinTF8](https://github.com/m13253/libWinTF8).

License
-------

This program is licensed under [GNU Lesser General Public License](https://www.gnu.org/copyleft/lgpl.html) 3.0 or newer version.
