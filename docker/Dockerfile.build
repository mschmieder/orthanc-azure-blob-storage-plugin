FROM ubuntu:16.04

ARG ORTHANC_VERSION=1.4.0
ENV ORTHANC_VERSION=${ORTHANC_VERSION}
ENV CXXFLAGS=-fPIC
ENV CFLAGS=-fPIC

RUN apt-get update -qq && apt-get install -y \
      wget \
      cmake \
      git \
      gcc \
      g++ \
      libgtest-dev \
      libjsoncpp-dev \
      libcrypto++-dev \
      libssl-dev \    
      libxml2-dev \
      uuid-dev \
      libboost-all-dev \
      zlib1g-dev \
      liblzma-dev \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

RUN git clone https://github.com/Microsoft/cpprestsdk.git -b v2.10.1

RUN cd /cpprestsdk/Release \
    && mkdir build.release \
    && cd build.release \
    && cmake .. \
      -DCMAKE_BUILD_TYPE=Release \
      -DBUILD_SHARED_LIBS=OFF \
      -DBUILD_TESTS=OFF \
      -DBUILD_SAMPLES=OFF \
      -DCMAKE_INSTALL_PREFIX=/cpprestsdk-dev \
    && make -j4 \
    && make install

# install latest cmake version
RUN wget -qq -O cmake_install.sh https://cmake.org/files/v3.12/cmake-3.12.0-Linux-x86_64.sh \
    && mkdir -p /opt/cmake \
    && bash cmake_install.sh --prefix=/opt/cmake --skip-license

    