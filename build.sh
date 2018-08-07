#!/bin/bash
set -e
SOURCE_DIR=$(pwd)

if [ -z "${ORTHANC_ROOT}" ];then
  ORTHANC_ROOT=$(find ${SOURCE_DIR} -name "Orthanc-${ORTHANC_VERSION}*" | sort -r | head -n1)
fi

BUILD_DIR=${BUILD_DIR:="${SOURCE_DIR}/build/"}
VCPKG_ROOT=${VCPKG_ROOT:="${SOURCE_DIR}/vcpkg"}
INSTALL_DIR=${INSTALL_DIR:="${SOURCE_DIR}/install/"}

if [ -n "${CPPRESTSDK_ROOT}" ]; then
  CMAKE_PREFIX_PATH="${CPPRESTSDK_ROOT};${CMAKE_PREFIX_PATH}"
  CMAKE_ADDITIONAL_OPTIONS="${CMAKE_ADDITIONAL_OPTIONS} -DCPPRESTSDK_ROOT=${CPPRESTSDK_ROOT}"
else
  CMAKE_PREFIX_PATH="/usr/lib/x86_64-linux-gnu/cmake/;${CMAKE_PREFIX_PATH}"
fi

if [ -n "${AZURESTORAGE_ROOT}" ]; then
  CMAKE_PREFIX_PATH="${AZURESTORAGE_ROOT};${CMAKE_PREFIX_PATH}"
  CMAKE_ADDITIONAL_OPTIONS="${CMAKE_ADDITIONAL_OPTIONS} -DAZURESTORAGE_ROOT=${AZURESTORAGE_ROOT}"
fi

if [ -n "${CMAKE_PREFIX_PATH}" ]; then
  CMAKE_ADDITIONAL_OPTIONS="${CMAKE_ADDITIONAL_OPTIONS} -DCMAKE_PREFIX_PATH=\"${CMAKE_PREFIX_PATH}\""
fi

# use the same cmake version that comes with VCPKG
# use the same cmake version that comes with VCPKG
if [ "$(uname)" == "Darwin" ]; then
  if [ -n "${USE_VCPKG}" ]; then
    CMAKE_BINARY=$(find ${VCPKG_ROOT} -type f -perm +111 -name cmake)
    VCPKG_CMAKE_OPTIONS="-DUSE_VCPKG:BOOL=ON -DCMAKE_TOOLCHAIN_FILE:PATH=${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
  fi
  OSX_CMAKE_OPTIONS="-DCMAKE_FIND_FRAMEWORK=LAST"

  export CXXFLAGS="-fPIC -Wno-cast-qual -Wno-pessimizing-move -Wno-unused-lambda-capture"
  export CFLAGS="-fPIC -Wno-cast-qual -Wno-pessimizing-move -Wno-unused-lambda-capture"
else
  if [ -n "${USE_VCPKG}" ]; then
    CMAKE_BINARY=$(find ${VCPKG_ROOT} -type f -executable -name cmake)
    VCPKG_CMAKE_OPTIONS="-DVCPKG_TARGET_TRIPLET=x64-linux-fpic -DUSE_VCPKG:BOOL=ON -DCMAKE_TOOLCHAIN_FILE:PATH=${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
  fi

  export CFLAGS="-fPIC"
  export CXXFLAGS="-fPIC"
fi

CMAKE_BINARY=${CMAKE_BINARY:=cmake}

mkdir -p ${BUILD_DIR}/{debug,release}
mkdir -p ${INSTALL_DIR}
echo "|------------------------------------------------------------------------------------------------------------------------------"
echo "|Configuration Setting:"
echo "|------------------------------------------------------------------------------------------------------------------------------"
echo "|  ORTHANC_ROOT:..............${ORTHANC_ROOT}"
echo "|  ORTHANC_VERSION:...........${ORTHANC_VERSION}"
echo "|  CXXFLAGS:..................${CXXFLAGS}"
echo "|  BUILD_DIR:.................${BUILD_DIR}"
echo "|  VCPKG_ROOT:................${VCPKG_ROOT}"
echo "|  INSTALL_DIR:...............${INSTALL_DIR}"
echo "|  CMAKE_BINARY:..............${CMAKE_BINARY}"
echo "|  CMAKE_PREFIX_PATH:.........${CMAKE_PREFIX_PATH}"
echo "|  CMAKE_ADDITIONAL_OPTIONS:..${CMAKE_PREFIX_PATH}"
echo "|  OSX_CMAKE_OPTIONS:.........${OSX_CMAKE_OPTIONS}"
echo "|  VCPKG_CMAKE_OPTIONS:.......${VCPKG_CMAKE_OPTIONS}"
echo "|------------------------------------------------------------------------------------------------------------------------------"

cd ${BUILD_DIR}/debug
${CMAKE_BINARY} ${SOURCE_DIR} -G"Unix Makefiles" \
      -DORTHANC_ROOT:PATH="${ORTHANC_ROOT}" \
      -DCMAKE_INSTALL_PREFIX:PATH=${INSTALL_DIR} \
      -DCMAKE_BUILD_TYPE=Debug \
      ${OSX_CMAKE_OPTIONS} \
      ${VCPKG_CMAKE_OPTIONS} \
      ${CMAKE_ADDITIONAL_OPTIONS}

${CMAKE_BINARY} --build . --config Debug --target install -- -j2

cd ${BUILD_DIR}/release
${CMAKE_BINARY} ${SOURCE_DIR} -G"Unix Makefiles" \
      -DORTHANC_ROOT:PATH="${ORTHANC_ROOT}" \
      -DCMAKE_INSTALL_PREFIX:PATH=${INSTALL_DIR} \
      -DCMAKE_BUILD_TYPE=Release \
      ${CMAKE_ADDITIONAL_OPTIONS} \
      ${OSX_CMAKE_OPTIONS} \
      ${VCPKG_CMAKE_OPTIONS} \
      
${CMAKE_BINARY} --build . --config Release --target install -- -j2
