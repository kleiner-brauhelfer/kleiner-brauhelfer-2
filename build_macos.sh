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
${QTDIR}/qmake "../${PRO}" -config release QMAKE_APPLE_DEVICE_ARCHS="x86_64 arm64" || exit 1
# make clean
make -j $(sysctl -n hw.logicalcpu) || exit 1
cd -

./deployment/macOS/deploy.sh "${BUILD_DIR}/bin/kleiner-brauhelfer-2.app" "${QTDIR}" "$2"|| exit 1
