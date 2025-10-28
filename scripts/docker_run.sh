#!/bin/bash

RPI_IP=$(/mnt/c/Windows/System32/WindowsPowerShell/v1.0/powershell.exe "Resolve-DnsName raspberrypi.local | Select-Object -ExpandProperty IPAddress" | tr -d '\r')

RPI_IP_V4=$(echo $RPI_IP | grep -oE '[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}' | head -n 1)

if [ -z "$RPI_IP_V4" ]; then
	echo "ERROR: Cannot resolve RPi IP."
	exit 1
fi
echo "Resolved RPi IP: $RPI_IP_V4"

HOST_PATH_ARMPI="$PWD/armpi/src"
HOST_PATH_MYAPP="$PWD/myapp/src"

echo "Mapping ARMPI from: $HOST_PATH_ARMPI"
echo "Mapping MYAPP from: $HOST_PATH_MYAPP"

HOSTNAME=$(hostname)

docker run -it --rm --name armpi_dev \
	--privileged \
	--net=host \
	-v "$HOST_PATH_ARMPI":/home/rosuser/ros_ws/armpi/src \
	-v "$HOST_PATH_MYAPP":/home/rosuser/ros_ws/myapp/src \
	-e ROS_MASTER_URI=http://${RPI_IP_V4}:11311 \
	--add-host $HOSTNAME:$127.0.0.1 --add-host raspberrypi:$RPI_IP_V4 \
	--add-host raspberrypi.local:$RPI_IP_V4 \
	armpi_env
