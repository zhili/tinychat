#! /bin/sh

aclocal -I /usr/local/share/aclocal/ \
&& automake --add-missing \
&& autoconf