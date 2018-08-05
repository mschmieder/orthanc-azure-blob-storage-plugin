FROM ubuntu:18.04

ARG USE_VCPKG=1
ENV USE_VCPKG=${USE_VCPKG}

ARG ORTHANC_VERSION=1.4.0
ENV ORTHANC_VERSION=${ORTHANC_VERSION}
ENV CXXFLAGS=-fPIC
ENV CFLAGS=-fPIC

RUN apt-get update -qq && apt-get install -y \
       wget \
       software-properties-common \
       cmake \
       git \
       gcc \
       g++ \
       curl \
       unzip \
       tar \
       sudo \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

COPY bootstrap.sh /
COPY triplets /triplets

RUN bash bootstrap.sh

#RUN rm -rf /vcpkg/buildtrees/*
#RUN rm -rf /vcpkg/downloads/*
RUN rm -rf /vcpkg/packages/*