#!/bin/bash

RPI_IP_V4="192.168.149.1"

HOST_PATH_ARMPI="$PWD/armpi/src"
HOST_PATH_MYAPP="$PWD/myapp/src"

echo "Mapping ARMPI from: $HOST_PATH_ARMPI"
echo "Mapping MYAPP from: $HOST_PATH_MYAPP"
echo "ROS_MASTER_URI:http://$RPI_IP_V4:11311"

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
