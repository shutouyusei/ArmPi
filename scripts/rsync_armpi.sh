rsync -avz --delete --exclude '.git' --exclude '__pycache__' --progress $PWD/armpi/src/ pi@raspberrypi.local:~/docker/src/armpi
