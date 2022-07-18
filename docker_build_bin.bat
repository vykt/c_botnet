@echo off

COPY  "%CD%\bin\master_elf" "%CD%\master\docker\"
COPY  "%CD%\bin\host_elf" "%CD%\host\docker"

docker "build" "-t" "debian-master:bullseye" "%CD%\master\docker"
docker "build" "-t" "debian-host:bullseye" "%CD%\host\docker"
docker "build" "-t" "debian-api:bullseye" "%CD%\api"
