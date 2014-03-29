#!/bin/bash

KERNEL="kernel"
BUILD="ASST3"
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

MOUNT=""
while (( "$#" )); do

	if [ "$1" == "-u" ]; then
		USER_PROGRAMS="T"
	elif [ "$1" == "-r" ]; then
		RUN="T"
	elif [ "$1" == "-w" ]; then
		DEBUG="T"
	elif [ "$1" == "-m" ]; then
		MOUNT="mount sfs FSVOL"
	fi
	shift
done

ARGS=""
if [ -n "$DEBUG" ]; then
	ARGS="-w"
fi

if [ -n "$USER_PROGRAMS" ]; then
	cd ../../..
	bmake
	bmake install
fi

if [ -n "$RUN" ]; then
	cp ~/csc369/CSCC69/a3/sys161.conf ~/csc369/root
	cd ~/csc369/root
	sys161 $ARGS $KERNEL "$MOUNT"
fi
