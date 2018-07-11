#!/bin/bash

TOPDIR=${TOPDIR:-$(git rev-parse --show-toplevel)}
SRCDIR=${SRCDIR:-$TOPDIR/src}
MANDIR=${MANDIR:-$TOPDIR/doc/man}

FOLMD=${FOLMD:-$SRCDIR/folmd}
FOLMCLI=${FOLMCLI:-$SRCDIR/folm-cli}
FOLMTX=${FOLMTX:-$SRCDIR/folm-tx}
FOLMQT=${FOLMQT:-$SRCDIR/qt/folm-qt}

[ ! -x $FOLMD ] && echo "$FOLMD not found or not executable." && exit 1

# The autodetected version git tag can screw up manpage output a little bit
FLMVER=($($FOLMCLI --version | head -n1 | awk -F'[ -]' '{ print $6, $7 }'))

# Create a footer file with copyright content.
# This gets autodetected fine for folmd if --version-string is not set,
# but has different outcomes for folm-qt and folm-cli.
echo "[COPYRIGHT]" > footer.h2m
$FOLMD --version | sed -n '1!p' >> footer.h2m

for cmd in $FOLMD $FOLMCLI $FOLMTX $FOLMQT; do
  cmdname="${cmd##*/}"
  help2man -N --version-string=${FLMVER[0]} --include=footer.h2m -o ${MANDIR}/${cmdname}.1 ${cmd}
  sed -i "s/\\\-${FLMVER[1]}//g" ${MANDIR}/${cmdname}.1
done

rm -f footer.h2m
