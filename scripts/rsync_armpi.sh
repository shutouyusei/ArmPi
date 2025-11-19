rsync -avz --delete --exclude '.git' --exclude '__pycache__' --progress $PWD/ros/armpi/ pi@raspberrypi.local:~/docker/src/armpi
