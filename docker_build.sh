##Docker build script

#Copy binaries
cp ./master/master_elf ./master/docker/
cp ./host/host_elf ./host/docker

#Build docker images
docker build -t debian-master:bullseye ./master/docker
docker build -t debian-host:bullseye ./host/docker
docker build -t debian-api:bullseye ./api
