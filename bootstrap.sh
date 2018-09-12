#!/bin/bash

set -e
ROOT_DIR=$(pwd)

# INSTALL VCPKG
if [ -n "${USE_VCPKG}" ]; then
  if [ ! -d "vcpkg" ];then
    git clone https://github.com/mschmieder/vcpkg.git
    cd vcpkg

    git checkout fix-boost-compile-flags

    ./bootstrap-vcpkg.sh

    cp -f ${ROOT_DIR}/triplets/* triplets/
  fi

  # use the same cmake version that comes with VCPKG
  if [ "$(uname)" == "Darwin" ]; then
    TRIPLET=""
  else
    as_sudo="sudo"
    TRIPLET="--triplet x64-linux-fpic"
  fi

  cd ${ROOT_DIR}

  # INSTALL REQUIRED LIBRARIES
  ${as_sudo} ./vcpkg/vcpkg ${TRIPLET} install \
    libuuid \
    liblzma \
    boost-uuid \
    azure-storage-cpp \
    jsoncpp \
    gtest \
    cryptopp
fi

# DOWNLOAD ORTHANC SOURCES
if [ "$(uname)" == "Darwin" ]; then
  ORTHANC_VERSION=${ORTHANC_VERSION:=1.3.1}
else
  ORTHANC_VERSION=${ORTHANC_VERSION:=1.4.1}
fi

if [ ! -d "orthanc" ];then
  ORHTANC_FILENAME=Orthanc-${ORTHANC_VERSION}.tar.gz
  wget -q -O ${ORHTANC_FILENAME} https://www.orthanc-server.com/downloads/get.php?path=/orthanc/${ORHTANC_FILENAME}
  tar -xzf ${ORHTANC_FILENAME}
  rm ${ORHTANC_FILENAME}
fi