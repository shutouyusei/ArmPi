#!/bin/bash

RPI_IP_V4="192.168.149.1"

HOST_PATH_ARMPI="$PWD/armpi/src"
HOST_PATH_MYAPP="$PWD/myapp/src"

echo "Mapping ARMPI from: $HOST_PATH_ARMPI"
echo "Mapping MYAPP from: $HOST_PATH_MYAPP"
echo "ROS_MASTER_URI:http://$RPI_IP_V4:11311"

case "$(uname -s)" in
Darwin)
	ROS_IP_ADDR=$(ipconfig getifaddr en0 2>/dev/null)
	if [ -z "$ROS_IP_ADDR" ]; then
		echo "Warning: Could not get IP from 'en0'. Trying 'en1'..."
		ROS_IP_ADDR=$(ipconfig getifaddr en1 2>/dev/null)
	fi
	;;
Linux)
	ROS_IP_ADDR=$(hostname -I | grep -oE '192\.168\.149\.[0-9]{1,3}' | awk '{print $1}')
	;;
*)
	echo "Error: Unsupported OS type. Please set ROS_IP_ADDR manually."
	exit 1
	;;
esac

if [ -z "$ROS_IP_ADDR" ]; then
	echo "Error: Failed to determine local IP address (ROS_IP) in the 192.168.149.x range."
	echo "Please ensure you are connected to the correct network."
	exit 1
fi
echo "Setting ROS_IP to: $ROS_IP_ADDR"

HOSTNAME=$(hostname)

docker run -it --rm --name armpi_dev \
	--privileged \
	--net=host \
	-v "$HOST_PATH_ARMPI":/home/rosuser/ros_ws/armpi/src \
	-v "$HOST_PATH_MYAPP":/home/rosuser/ros_ws/myapp/src \
	-e ROS_MASTER_URI=http://${RPI_IP_V4}:11311 \
	--add-host $HOSTNAME:127.0.0.1 --add-host raspberrypi:$RPI_IP_V4 \
	--add-host raspberrypi.local:$RPI_IP_V4 \
	armpi_env
