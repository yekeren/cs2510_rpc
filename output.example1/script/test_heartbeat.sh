#!/bin/sh

set -x

test -d log || mkdir log
nohup ./bin/directory_server -p 8000 > log/directory_server.log 2>&1 &

name=`ls server_stub/bin`
cp server_stub/bin/${name} bin

nohup ./bin/${name} -i 127.0.0.1 -p 8001 > log/server.1.log 2>&1 &
sleep 2
nohup ./bin/${name} -i 127.0.0.1 -p 8002 > log/server.2.log 2>&1 &
sleep 10

exit 0
