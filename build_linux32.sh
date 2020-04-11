#!/bin/sh

QT_DIR=$1
DEPLOY=$2

if [ "${QT_DIR}" = "" ]; then
  echo "Usage: build_linux32.sh <qt-bin-directory> [deploy]"
  exit 1
fi

PRO=$(dirname $0)/kleiner-brauhelfer-2.pro
BUILD_DIR=$(dirname $0)/build-linux32

mkdir ${BUILD_DIR}
cd ${BUILD_DIR}
"${QT_DIR}/qmake" "../${PRO}" -config release
#make clean
make -j 8
#"${QT_DIR}/lupdate" "../${PRO}"
#"${QT_DIR}/lrelease" "../${PRO}"
cd -

if [ ${DEPLOY} = "1" ]; then
  ./deployment/linux/32bit/deploy.sh "${BUILD_DIR}/bin" "${QT_DIR}"
fi
