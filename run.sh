#!/bin/bash

PROJECT_DIR=$(pwd)
BUILD=build
CMAKE=/usr/local/bin/cmake

if [ ! -d $BUILD ]; then
	mkdir $BUILD
	cd $BUILD	
else
	cd $BUILD
fi


if [ ! -f $CAMKE ]; then
    echo "Please install cmake"
else
	cmake .. -DCMAKE_C_COMPILER=gcc
	make -j 5
	Server
	Client
fi
