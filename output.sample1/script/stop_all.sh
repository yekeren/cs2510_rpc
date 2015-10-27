#!/bin/sh

set -x

test -d log || mkdir log

name=`ls server_stub/bin`

killall ${name}
killall directory_server

exit 0
