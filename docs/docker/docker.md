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
sudo bash scripts/docker_run.sh
```

## How to enter bash from other terminals

```
docker exec -it armpi_dev bash
```

:::note warn
after enter bash, you must use `source /opt/ros/noetic/setup.bash`
:::
