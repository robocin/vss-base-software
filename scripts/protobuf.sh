#!/bin/bash

function is_root {
  [ "${EUID:-$(id -u)}" -eq 0 ];
}

if ! is_root; then
  echo -e "\x1B[31m[ERROR] This script requires root privileges."
  exit 1
fi

set -e

PROTOBUF_VERSION="3.7.1"
PROTOBUF_DIR="protobuf-${PROTOBUF_VERSION}"
PROTOBUF_ARCHIVE="v${PROTOBUF_VERSION}.tar.gz"
PROTOBUF_URL="https://github.com/protocolbuffers/protobuf/archive/${PROTOBUF_ARCHIVE}"

echo "Installing Protobuf ${PROTOBUF_VERSION}..."

apt install -y autoconf automake libtool curl make g++ unzip

echo "Downloading Protobuf ${PROTOBUF_VERSION}..."
wget ${PROTOBUF_URL}

echo "Extracting files..."
tar -xvf ${PROTOBUF_ARCHIVE}
cd ${PROTOBUF_DIR}

./autogen.sh

./configure

make -j$(nproc)
make install
ldconfig 

cd ..
rm -rf ${PROTOBUF_DIR} ${PROTOBUF_ARCHIVE}

echo "Protobuf ${PROTOBUF_VERSION} installation completed successfully!"
