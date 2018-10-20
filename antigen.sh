#!/bin/sh

set -x
if test -f Makefile; then
  make distclean
fi
rm -Rf autom4te.cache
rm -f *.tar.* *.tgz config.status.lineno ChangeLog NEWS README aclocal.m4 configure compile depcomp install-sh missing ltmain.sh config.guess config.sub config.h.in mkinstalldirs INSTALL config.log config.status libtool config.h.in~ stamp-h1 config.h Makefile
find . -name Makefile.in | xargs rm -f 
