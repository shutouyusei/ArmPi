#!/bin/bash

RPI_IP_V4="192.168.149.1"

HOST_PATH_MYAPP="$PWD/ros/myapp"
HOST_PATH_SHARE="$PWD/ros/share"
HOST_PATH_DATA="$PWD/datasets"
HOST_PATH_MODEL="$PWD/models"

echo "Mapping ARMPI from: $HOST_PATH_ARMPI"
echo "Mapping MYAPP from: $HOST_PATH_MYAPP"
echo "Mapping SHARE from: $HOST_PATH_SHARE"
echo "ROS_MASTER_URI:http://$RPI_IP_V4:11311"

HOSTNAME=$(hostname)

docker run -it --rm --name armpi_dev \
	--privileged \
	--net=host \
	-v "/tmp/.X11-unix:/tmp/.X11-unix" \
	-e DISPLAY=$DISPLAY \
	-v "$HOST_PATH_MYAPP":/home/rosuser/ros_ws/src/myapp \
	-v "$HOST_PATH_SHARE":/home/rosuser/ros_ws/src/share \
	-v "$HOST_PATH_DATA":/home/rosuser/ros_ws/datasets \
	-v "$HOST_PATH_MODEL":/home/rosuser/ros_ws/models \
	-e ROS_MASTER_URI=http://${RPI_IP_V4}:11311 \
	--add-host $HOSTNAME:127.0.0.1 --add-host raspberrypi:$RPI_IP_V4 \
	--add-host raspberrypi.local:$RPI_IP_V4 \
	armpi_env
