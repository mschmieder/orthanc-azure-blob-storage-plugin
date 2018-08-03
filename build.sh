#!/bin/bash
set -e
SOURCE_DIR=$(pwd)

if [ -z "${ORTHANC_ROOT}" ];then
  ORTHANC_ROOT=$(find ${SOURCE_DIR} -name "Orthanc-*" | sort -r | head -n1)
fi

if [ -z "${BUILD_DIR}" ]; then
  BUILD_DIR=${SOURCE_DIR}/build/
fi

if [ -z "${VCPKG_ROOT}" ]; then
   VCPKG_ROOT=${SOURCE_DIR}/vcpkg
 fi

if [ -z "${INSTALL_DIR}" ]; then
  INSTALL_DIR=${SOURCE_DIR}/install/
fi

if [ -n "${CPPRESTSDK_ROOT}" ]; then
  CMAKE_ADDITIONAL_OPTIONS="${CMAKE_ADDITIONAL_OPTIONS} -DCMAKE_PREFIX_PATH=${CPPRESTSDK_ROOT}"
fi

# use the same cmake version that comes with VCPKG
# use the same cmake version that comes with VCPKG
if [ "$(uname)" == "Darwin" ]; then
  if [ -n "${USE_VCPKG}" ]; then
    CMAKE_BINARY=$(find ${VCPKG_ROOT} -type f -perm +111 -name cmake)
    vcpkg_cmake_options="-DUSE_VCPKG:BOOL=ON -DCMAKE_TOOLCHAIN_FILE:PATH=${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
    osx_cmake_options="-DCMAKE_FIND_FRAMEWORK=LAST"
  fi
else
  if [ -n "${USE_VCPKG}" ]; then
    CMAKE_BINARY=$(find ${VCPKG_ROOT} -type f -executable -name cmake)
  fi
  vcpkg_cmake_options="-DVCPKG_TARGET_TRIPLET=x64-linux-fpic -DUSE_VCPKG:BOOL=ON -DCMAKE_TOOLCHAIN_FILE:PATH=${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
fi

if [ -z "${CMAKE_BINARY}" ]; then
  CMAKE_BINARY=cmake
fi



mkdir -p ${BUILD_DIR}/{debug,release}
mkdir -p ${INSTALL_DIR}

cd ${BUILD_DIR}/debug
${CMAKE_BINARY} ${SOURCE_DIR} -G"Unix Makefiles" \
      -DORTHANC_ROOT:PATH="${ORTHANC_ROOT}" \
      -DCMAKE_INSTALL_PREFIX:PATH=${INSTALL_DIR} \
      -DCMAKE_BUILD_TYPE=Debug \
      ${osx_cmake_options} \
      ${vcpkg_cmake_options} \
      ${CMAKE_ADDITIONAL_OPTIONS}

${CMAKE_BINARY} --build . --config Debug --target install -- -j2

cd ${BUILD_DIR}/release
${CMAKE_BINARY} ${SOURCE_DIR} -G"Unix Makefiles" \
      -DORTHANC_ROOT:PATH="${ORTHANC_ROOT}" \
      -DCMAKE_INSTALL_PREFIX:PATH=${INSTALL_DIR} \
      -DCMAKE_BUILD_TYPE=Release \
      ${osx_cmake_options} \
      ${vcpkg_cmake_options} \
      ${CMAKE_ADDITIONAL_OPTIONS}

${CMAKE_BINARY} --build . --config Release --target install -- -j2