#!/bin/sh
# autogen.sh
#
# Rebuild the autotools build system 

echo "Rebuilding autotools build system:"
autoheader
aclocal
libtoolize --force
autoconf
automake -Wall --add-missing
echo
echo "To rebuild generated codelets (~ 5 minute wait):"
echo "  ./configure --enable-maintainer-mode && make"
echo
