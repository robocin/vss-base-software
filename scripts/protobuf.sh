#!/bin/bash

PROTOBUF_FILE="protobuf-cpp-3.7.1.tar.gz"
PROTOBUF_DIR="protobuf-3.7.1"
PROTOBUF_URL="https://github.com/protocolbuffers/protobuf/releases/download/v3.7.1/${PROTOBUF_FILE}"

echo "Installing Protobuf..."

sudo apt install -y autoconf automake libtool wget make g++ unzip

echo "Downloading Protobuf..."
wget ${PROTOBUF_URL}

echo "Extracting files..."
tar -xvf ${PROTOBUF_FILE}
cd ${PROTOBUF_DIR}

sudo ./autogen.sh

sudo ./configure

sudo make -j$(nproc)
sudo make install
sudo ldconfig 

cd ..
rm -rf ${PROTOBUF_DIR}
rm -rf ${PROTOBUF_FILE}

echo "Protobuf installation completed successfully!"
