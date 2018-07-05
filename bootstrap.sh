#!/bin/bash
root_dir=$(pwd)

# Install VCPKG
if [ ! -d vcpkg ];then
  git clone https://github.com/Microsoft/vcpkg.git
  cd vcpkg
  ./bootstrap-vcpkg.sh
  cd ${root_dir}

  export PATH=${root_dir}/vcpkg:${PATH}
fi 

# Install required libraries
vcpkg install azure-storage-cpp jsoncpp gtest

mkdir build
cd build

cmake ${root_dir} -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=${root_dir}/vcpkg/scripts/buildsystems/vcpkg.cmake