rsync -avz --delete --exclude '.git' --exclude '__pycache__' --progress $PWD/share/src/ pi@raspberrypi.local:~/docker/src/share
