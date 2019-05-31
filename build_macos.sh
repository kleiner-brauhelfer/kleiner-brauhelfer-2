#!/bin/sh

QTDIR=$1
if [ "${QTDIR}" = "" ]; then
  echo  "Usage: $0 <qt-bin-directory> <optional version_suffix>"
  exit 1
fi

PRO="kleiner-brauhelfer-2.pro"

${QTDIR}/qmake "${PRO}" -config release || exit 1
make clean && make || exit 1
${QTDIR}/lupdate "${PRO}" || exit 1
${QTDIR}/lrelease "${PRO}" || exit 1
./deployment/macOS/deploy.sh ./bin/kleiner-brauhelfer.app "${QTDIR}" "$2"|| exit 1
