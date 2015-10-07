#!/bin/sh

killall ds
killall cs

tail -F log/*.log
