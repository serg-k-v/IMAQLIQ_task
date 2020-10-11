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

	./Server -p 5006 -f ../client_data.txt &
	server_pid=$!
	./Client  0.0.0.0 5006
	client_pid=$! 
	sleep 2s
	kill -15 $server_pid
fi
