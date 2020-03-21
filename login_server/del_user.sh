#!/bin/bash

sed -i `grep -n -w "${1}" userfile | awk -F : '{print $1}'`d userfile
