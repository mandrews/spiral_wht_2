#!/bin/sh
# autogen.sh
#
# [Re]generate the autotools build system 

autoheader
aclocal
libtoolize
autoconf
automake --add-missing
echo "To [re]build generated codelets:"
echo "  ./configure --enable-maintainer-mode && make dist"
echo "NOTE: this takes a few minutes"
