#!/bin/bash
set -e
SOURCE_DIR=$(pwd)

if [ -z "${ORTHANC_ROOT}" ];then
  ORTHANC_ROOT=$(find $(pwd) -name Orthanc-* | head -n1)
fi

if [ -z "${BUILD_DIR}" ]; then
  BUILD_DIR=${SOURCE_DIR}/build/
fi

if [ -z "${INSTALL_DIR}" ]; then
  INSTALL_DIR=${SOURCE_DIR}/install/
fi

if [ -z "${CMAKE_BINARY}" ]; then
  cmake_exec=cmake
else
  cmake_exec=${CMAKE_BINARY}
fi

if [ -n "${CPPRESTSDK_ROOT}" ]; then
  CMAKE_ADDITIONAL_OPTIONS="${CMAKE_ADDITIONAL_OPTIONS} -DCMAKE_PREFIX_PATH=${CPPRESTSDK_ROOT}"
fi

# use the same cmake version that comes with VCPKG
if [ "$(uname)" == "Darwin" ]; then
  osx_cmake_options="-DCMAKE_FIND_FRAMEWORK=LAST"
fi

mkdir -p ${BUILD_DIR}/{debug,release}
mkdir -p ${INSTALL_DIR}

cd ${BUILD_DIR}/debug
${cmake_exec} ${SOURCE_DIR} -G"Unix Makefiles" \
      -DORTHANC_ROOT:PATH="${ORTHANC_ROOT}" \
      -DCMAKE_INSTALL_PREFIX:PATH=${INSTALL_DIR} \
      -DCMAKE_BUILD_TYPE=Debug \
      ${osx_cmake_options} \
      ${CMAKE_ADDITIONAL_OPTIONS}

${cmake_exec} --build . --config Debug --target install -- -j

cd ${BUILD_DIR}/release
${cmake_exec} ${SOURCE_DIR} -G"Unix Makefiles" \
      -DORTHANC_ROOT:PATH="${ORTHANC_ROOT}" \
      -DCMAKE_INSTALL_PREFIX:PATH=${INSTALL_DIR} \
      -DCMAKE_BUILD_TYPE=Release \
      ${osx_cmake_options} \
      ${CMAKE_ADDITIONAL_OPTIONS}

${cmake_exec} --build . --config Release --target install -- -j