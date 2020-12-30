#!/bin/sh
$EXTRACTRC `find . -name \*.rc -o -name \*.ui -o -name \*.kcfg` >> rc.cpp
$XGETTEXT *.cpp -o $podir/ksnakeduel.pot
rm -f rc.cpp
