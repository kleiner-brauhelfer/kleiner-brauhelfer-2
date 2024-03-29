#!/bin/sh

QT_DIR=$1
if [ "${QT_DIR}" = "" ]; then
  echo "Usage: build_linux64.sh <qt-bin-directory>"
  exit 1
fi

SCRIPT_DIR=$(dirname $(readlink -f $0))
PRO=${SCRIPT_DIR}/kleiner-brauhelfer-2.pro
BUILD_DIR=${SCRIPT_DIR}/build-linux64

mkdir -p ${BUILD_DIR}
cd ${BUILD_DIR}
"${QT_DIR}/qmake" "${PRO}" -config release || exit 1
make -j $(nproc)
if [ $? -ne 0 ]; then
  echo "Failed to build." >&2
  exit 1
fi
cd -

${SCRIPT_DIR}/deployment/linux/64bit/deploy.sh "${BUILD_DIR}/bin" "${QT_DIR}"

exit 0
