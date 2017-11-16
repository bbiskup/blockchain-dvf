docker-compose build
docker-compose up -d
./docker-cmd.sh "mkdir -p build && cd build && cmake -G Ninja .. && ninja"
