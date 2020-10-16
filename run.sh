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

	# ./Imaqliq_Server -p $port -f ../client_data.txt &
	# server_pid=$!
	# ./Client  0.0.0.0 $port ../send_data.txt
	# client_pid=$!

	SERVICE_DIR=$PROJECT_DIR/service_files
	SERVICE_CONF_PATH=/etc/systemd/system
	LOCAL_CHANGE=flase

	if ! cmp -s "$SERVICE_DIR/imaqliq-server.service" "$SERVICE_CONF_PATH/imaqliq-server.service" \
		|| [ ! -f $SERVICE_DIR/imaqliq-server.service ]; then
		sudo cp $SERVICE_DIR/imaqliq-server.service $SERVICE_CONF_PATH
		LOCAL_CHANGE=true
	else
		echo ".services equals"
	fi

	if ! cmp -s "$SERVICE_DIR/imaqliq-server.socket" "$SERVICE_CONF_PATH/imaqliq-server.socket"  \
		|| [ ! -f $SERVICE_DIR/imaqliq-server.socket ]; then
		sudo cp $SERVICE_DIR/imaqliq-server.socket  $SERVICE_CONF_PATH
		LOCAL_CHANGE=true
	else
		echo ".sockets equals"
	fi

	if [ $LOCAL_CHANGE == true ]; then
		sleep 5s
		systemctl daemon-reload
	fi

	if [ ! -d /tmp/imaqliq_task ]; then
		mkdir /tmp/imaqliq_task
	fi

	if ! cmp -s -- "./Imaqliq_Server" "/tmp/imaqliq_task/imaqliq_serverd" ; then
		if [ "$(systemctl is-active imaqliq-server.service)" == "active" ]; then
			echo "imaqliq-server is active. Stoping service."
			systemctl stop imaqliq-server.service
		else
			cp ./Imaqliq_Server /tmp/imaqliq_task/imaqliq_serverd
		fi
	else
		echo "Imaqliq_Server bin equals /tmp/imaqliq_task/imaqliq_serverd"
	fi


	if [ "$(systemctl is-enabled imaqliq-server.socket)" == "disabled" ]; then
		systemctl enable imaqliq-server.socket
	fi

	# systemctl start  imaqliq-server.socket

	if [ "$(systemctl is-enabled imaqliq-server.service)" == "disabled" ]; then
		systemctl enable imaqliq-server.service
	fi

	if [ "$(systemctl is-active imaqliq-server.service)" == "inactive" ]; then
		systemctl start  imaqliq-server.service
	fi

	sleep 5s
	systemctl daemon-reload

	sleep 2s

	# if [ signal > 0 ]; then
	# 	kill -$signal $server_pid
	# fi
fi
