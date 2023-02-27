#!/bin/sh

QT_DIR=$1
DEPLOY=$2

if [ "${QT_DIR}" = "" ]; then
  echo "Usage: build_linux64.sh <qt-bin-directory> [deploy]"
  exit 1
fi

PRO=$(dirname $0)/kleiner-brauhelfer-2.pro
BUILD_DIR=$(dirname $0)/build-linux64

mkdir ${BUILD_DIR}
cd ${BUILD_DIR}
"${QT_DIR}/qmake" "../${PRO}" -config release
make -j 8
if [ $? -ne 0 ]; then
  echo "Failed to build." >&2
  exit 1
fi
cd -

if [ ${DEPLOY} = "1" ]; then
  ./deployment/linux/64bit/deploy.sh "${BUILD_DIR}/bin" "${QT_DIR}"
fi

exit 0
