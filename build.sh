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
ORTHANC_VERSION=1.3.1
if [ ! -d "orthanc" ];then
  ORHTANC_FILENAME=Orthanc-${ORTHANC_VERSION}.tar.gz
  wget -q -O ${ORHTANC_FILENAME} https://www.orthanc-server.com/downloads/get.php?path=/orthanc/${ORHTANC_FILENAME}
  tar -xzf ${ORHTANC_FILENAME}
  rm ${ORHTANC_FILENAME}
fi

mkdir -p ${SOURCE_DIR}/build/{debug,release}
mkdir -p ${SOURCE_DIR}/install

cd ${SOURCE_DIR}/build/debug
cmake ${SOURCE_DIR} -G"Unix Makefiles" \
      -DORTHANC_ROOT:PATH="${SOURCE_DIR}/Orthanc-${ORTHANC_VERSION}" \
      -DCMAKE_TOOLCHAIN_FILE:PATH=${SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake \
      -DCMAKE_INSTALL_PREFIX:PATH=${SOURCE_DIR}/install \
      -DCMAKE_BUILD_TYPE=Debug

cmake --build . --config Debug --target install -- -j

cd ${SOURCE_DIR}/build/release
cmake ${SOURCE_DIR} -G"Unix Makefiles" \
      -DORTHANC_ROOT:PATH="${SOURCE_DIR}/Orthanc-${ORTHANC_VERSION}" \
      -DCMAKE_TOOLCHAIN_FILE:PATH=${SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake \
      -DCMAKE_INSTALL_PREFIX:PATH=${SOURCE_DIR}/install \
      -DCMAKE_BUILD_TYPE=Release

cmake --build . --config Release --target install -- -j