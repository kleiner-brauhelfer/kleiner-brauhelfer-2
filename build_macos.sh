#!/bin/sh

QTDIR=$1
if [ "${QTDIR}" = "" ]; then
  echo  "Usage: build_macos.sh <qt-bin-directory> <optional version_suffix>"
  exit 1
fi

PRO="kleiner-brauhelfer-2.pro"
BUILD_DIR=$(dirname $0)/build-macos

mkdir -p ${BUILD_DIR}
cd ${BUILD_DIR}
${QTDIR}/qmake "../${PRO}" -config release || exit 1
# make clean
make -j 8 || exit 1
#${QTDIR}/lupdate "${PRO}" || exit 1
#${QTDIR}/lrelease "${PRO}" || exit 1
cd -

./deployment/macOS/deploy.sh "${BUILD_DIR}/bin/kleiner-brauhelfer-2.app" "${QTDIR}" "$2"|| exit 1
