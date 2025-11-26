#!/bin/sh

QT_DIR=$1
if [ "${QT_DIR}" = "" ]; then
  echo "Usage: build_openbsd64.sh <qt-bin-directory>"
  exit 1
fi

SCRIPT_DIR=$(dirname $(readlink -f $0))
PRO=${SCRIPT_DIR}/kleiner-brauhelfer-2.pro
BUILD_DIR=${SCRIPT_DIR}/build-openbsd64

mkdir -p ${BUILD_DIR}
cd ${BUILD_DIR}
"${QT_DIR}/qmake-qt6" "${PRO}" -config release || exit 1
gmake -j $(sysctl -n hw.ncpu)
if [ $? -ne 0 ]; then
  echo "Failed to build." >&2
  exit 1
fi
cd -