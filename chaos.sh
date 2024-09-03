#!/bin/bash

current_directory=${PWD}

cd ./scripts/ || exit

bash chaos.sh "$@"

cd "$current_directory" || exit
