# Docker

## create image

```
docker build -t armpi_env .
```

if you want to create image for RaspberryPi
```
docker buildx build --platform linux/arm64 -t armpi_env:latest-arm64 .
```

## create container

```
docker run -it --rm \
  --name armpi_dev \
  --privileged \
  --net=host \
  -v $(pwd)/armpi:/root/ros_ws/src/armpi \
  armpi_env 
```

## How to enter bash from other terminals

```
docker exec -it armpi_dev bash
```

:::note warn
after enter bash, you must use `source /opt/ros/noetic/setup.bash`
:::
