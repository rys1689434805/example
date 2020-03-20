#!/bin/bash

docker ps -f ancestor=game:V1.0 | grep game_start | awk -F "->" '{print $1}' | awk -F : '{print $NF}' | grep "$1" 2>&1 > /dev/null
