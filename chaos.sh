#!/bin/bash

current_directory=${PWD}

cd ./scripts/

bash chaos.sh "$@"

cd "$current_directory"
