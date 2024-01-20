#!/bin/bash

if [[ -z "$1" ]]
then
cat << EOF
Usage $0 [arg]
  Arguments:
    build:                 Compile binaries
    rebuild:               Runs clean first and then rebuilds os
    clean [option]:        Clean build directory
                           empty removes all built binaries and images
                           image only removes built image
                           code removes built binaries

    run-image (default):   Run os with qemu image
    debug-image:           Debug os with qemu image (runs cmake build in background to update binaries)

	run-image: Run kernel in qemu with image
EOF
fi

clean() {
  bash ./clean.sh $1
}

build_os_image()
{
  echo "Building os image"
  bash ./build.sh
  
  if [ -f "./build/chaos.img" ]
  then
    bash ./mount_image.sh
  else
    bash ./create_image.sh
  fi
  
  bash ./sync_image.sh
  
  bash ./unmount_image.sh
}

run_image()
{
  build_os_image

  echo "Starting qemu with os image"
  bash ./qemu_image.sh $1
}

if [ "$1" == "build" ]
then
  build_os_image
fi

if [ "$1" == "rebuild" ]
then
  clean
  build_os_image
fi

if [ "$1" == "run-image" ]
then
  run_image
fi

if [ "$1" == "debug-image" ]
then
  run_image debug
fi
