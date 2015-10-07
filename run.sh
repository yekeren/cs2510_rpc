#!/bin/sh

nohup ./svr_ds -p 8000 > ./log/ds.log 2>&1 &
nohup ./svr_cs -p 8001 > ./log/cs.log 2>&1 &

tail -F log/*.log
