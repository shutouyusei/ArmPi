FROM ros:noetic-robot

# install libary
RUN apt-get update && apt-get install -y \
    python3-pip \
    ros-noetic-serial \
    && rm -rf /var/lib/apt/lists/*

RUN pip3 install RPi.GPIO
RUN pip3 install numpy pyserial

# ROS WorkSpace
WORKDIR /root/ros_ws
RUN mkdir src
RUN /bin/bash -c "source /opt/ros/noetic/setup.bash; catkin_init_workspace" 
