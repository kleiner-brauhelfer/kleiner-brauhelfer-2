#!/bin/sh

QTDIR=$1
if [ "${QTDIR}" = "" ]; then
  echo  "Usage: build.sh <qt-bin-directory> <optional version_suffix>"
  exit 1
fi

PRO="kleiner-brauhelfer-2.pro"

# comment out #define DEBUG true
#sed -i '' -e '/^#define DEBUG true/ s/^#*/\/\/#/' ./source/src/definitionen.h

${QTDIR}/qmake "${PRO}" || exit 1
make clean && make || exit 1
${QTDIR}/lupdate "${PRO}" || exit 1
${QTDIR}/lrelease "${PRO}" || exit 1
./deployment/macOS/deploy.sh ./bin/kleiner-brauhelfer.app "${QTDIR}" "$1"|| exit 1
