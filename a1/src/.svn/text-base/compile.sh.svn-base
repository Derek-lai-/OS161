#!/bin/bash

if [ -z "$1" ]; then
	BUILD="ASST1"
else
	BUILD=$1
fi
echo $BUILD

cd ../..

# cd to the conf directory
cd a1/src/kern/conf

# create some config files for ASST1
./config $BUILD

# compile the actual kernel
cd ../compile/$BUILD
bmake depend
echo "[INFO] Compiling"
bmake
echo "[INFO] Installing the kernel"
bmake install
