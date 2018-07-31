#!/bin/bash

# DOWNLOAD ORTHANC SOURCES
if [ -z "${ORTHANC_VERSION}" ];then
  ORTHANC_VERSION=1.3.1
fi

if [ ! -d "orthanc" ];then
  ORHTANC_FILENAME=Orthanc-${ORTHANC_VERSION}.tar.gz
  wget -q -O ${ORHTANC_FILENAME} https://www.orthanc-server.com/downloads/get.php?path=/orthanc/${ORHTANC_FILENAME}
  tar -xzf ${ORHTANC_FILENAME}
  rm ${ORHTANC_FILENAME}
fi