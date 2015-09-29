#!/bin/sh

set -x

unregister() {
curl -v -d "<server><id>$1</id><version>$2</version><name>$3</name><ip>$4</ip><port>$5</port></server>" "http://127.0.0.1:8001/unregister"
}

unregister "2" "1.0.0" "comp_svr" "127.0.0.1" "8001"
unregister "1" "1.0.0" "comp_svr" "127.0.0.1" "8001"
exit 0
