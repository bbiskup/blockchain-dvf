#!/bin/bash

# Run command in running container.
# Start the container with 'docker-compose up' first

if [ -n "$TERM" ];then
    EXEC_OPTS="-ti"
fi

IMAGE_NAME=blockchaindvf_dev_1

docker exec -u root $IMAGE_NAME bash -c "useradd -u$(id -u) containeruser" 2> /dev/null
docker exec -u containeruser $EXEC_OPTS $IMAGE_NAME bash -c "$@"

