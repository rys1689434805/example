#!/bin/bash

docker run --rm -d -P game:V1.0 2>&1 >/dev/null

docker ps -n 1 -f ancestor=game:V1.0 | grep game_start | awk -F "->" '{print $1}' | awk -F : '{print $NF}'
