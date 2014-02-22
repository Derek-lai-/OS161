#!/bin/bash

if [ -z "$1" ]; then
	echo "Usage: ./compile.sh BUILD-NAME (ASST2-NORAND (-nr) or ASST2-RAND (-r))"
	exit -1;
elif [ "$1" == "-r" ]; then
	BUILD="ASST2-RAND"
	KERNEL="kernel-ASST2-RAND"
elif [ "$1" == "-nr" ]; then
	BUILD="ASST2-NORAND"
	KERNEL="kernel-ASST2-NORAND"
elif [ "$1" != "ASST2-NORAND" -o "$1" != "ASST2-RAND" ]; then
	echo "Wrong parameter! Can only be ASST2-NORAND (-nr) or ASST2-RAND (-r)!"
	exit -2;
else
	BUILD=$1
fi
ARGS=""
echo $BUILD

# cd to the conf directory
cd kern/conf

# create some config files for ASST2
./config $BUILD

# compile the actual kernel
cd ../compile/$BUILD
bmake depend
echo "[INFO] Compiling"
bmake
echo "[INFO] Installing the kernel"
bmake install

# if the third argument is -w then debug the system
if [ "$3" == "-w" ]; then
	ARGS="-w"
fi

# if the second argument is -s then run the system
if [ "$2" == "-s" ]; then
	cp sys161.conf ~/csc369/root
	cd ~/csc369/root
	sys161 $ARGS $KERNEL
fi
