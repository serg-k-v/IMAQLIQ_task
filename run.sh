#!/bin/bash

PROJECT_DIR=$(pwd)
BUILD=build
CMAKE=/usr/local/bin/cmake
OPTIND=1
signal=0
port=0

function show_help {
	echo "Use -s <number signal> -p <port number> for kill server process after data transfering"
}

while getopts "h?s:p:" opt; do
    case "$opt" in
    h|\?)
        show_help
        exit 0
        ;;
    s)  signal=$OPTARG
        ;;
	p)  port=$OPTARG 
    esac
done

shift $((OPTIND-1))
[ "${1:-}" = "--" ] && shift

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

	echo "-------BUILD IS DONE -----------"

	./Server -p $port -f ../client_data.txt &
	server_pid=$!
	./Client  0.0.0.0 $port ../client_data.txt
	client_pid=$!

	sleep 2s

	if [ signal > 0 ]; then
		kill -$signal $server_pid
	fi
fi
