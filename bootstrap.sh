#!/bin/bash

set -e

SOURCE_DIR=$(pwd)

# INSTALL VCPKG
if [ ! -d vcpkg ];then
  git clone https://github.com/Microsoft/vcpkg.git
  cd vcpkg
  ./bootstrap-vcpkg.sh
  cd ${SOURCE_DIR}

  export PATH=${SOURCE_DIR}/vcpkg:${PATH}
fi 

# INSTALL REQUIRED LIBRARIES
vcpkg install azure-storage-cpp jsoncpp gtest


# DOWNLOAD ORTHANC SOURCES
ORTHANC_VERSION=1.3.2
if [ ! -d "orthanc" ];then
  ORHTANC_FILENAME=Orthanc-${ORTHANC_VERSION}.tar.gz
  wget -q -O ${ORHTANC_FILENAME} https://www.orthanc-server.com/downloads/get.php?path=/orthanc/${ORHTANC_FILENAME}
  tar -xzf ${ORHTANC_FILENAME}
  rm ${ORHTANC_FILENAME}
fi

mkdir -p ${SOURCE_DIR}/build/{release,debug}

cd ${SOURCE_DIR}/build/debug
cmake ${SOURCE_DIR} -G"Unix Makefiles" \
      -DORTHANC_ROOT:PATH="${SOURCE_DIR}/Orthanc-${ORTHANC_VERSION}" \
      -DCMAKE_BUILD_TYPE:STRING=Debug \
      -DCMAKE_TOOLCHAIN_FILE:PATH=${SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake

cd ${SOURCE_DIR}/build/release
cmake ${SOURCE_DIR} -G"Unix Makefiles" \
      -DORTHANC_ROOT:PATH="${SOURCE_DIR}/Orthanc-${ORTHANC_VERSION}" \
      -DCMAKE_BUILD_TYPE:STRING=Release \
      -DCMAKE_TOOLCHAIN_FILE:PATH=${SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake