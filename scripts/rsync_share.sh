rsync -avz --delete --exclude '.git' --exclude '__pycache__' --progress $PWD/ros/share/ pi@raspberrypi.local:~/docker/src/share
