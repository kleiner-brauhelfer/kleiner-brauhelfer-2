#!/bin/sh

QT_DIR=$1
PORTABLE=$2
DEPLOY=$3

if [ "${QT_DIR}" = "" ]; then
  echo "Usage: build_linux64.sh <qt-bin-directory> [portable] [deploy]"
  exit 1
fi

PRO=$(dirname $0)/kleiner-brauhelfer-2.pro
BUILD_DIR=$(dirname $0)/build-linux64

mkdir ${BUILD_DIR}
cd ${BUILD_DIR}
"${QT_DIR}/qmake" "../${PRO}" -config release
make clean
make -j 8
"${QT_DIR}/lupdate" "../${PRO}"
"${QT_DIR}/lrelease" "../${PRO}"
cd ..

if [ ! -z ${PORTABLE} ]; then
  touch "${BUILD_DIR}/bin/portable"
fi

if [ ! -z ${DEPLOY} ]; then
  ./deployment/linux/64bit/deploy.sh "${BUILD_DIR}/bin" "${QT_DIR}"
fi
