#!/bin/bash

source ~/i686_cross_compiler/i686.env
if ! type "i686-elf-gcc" &> /dev/null;
then
  echo "Cannot find i686-elf-gcc compiler"
  while true; do
    # shellcheck disable=SC2162
    read -p "Do you wish to automatically install i686-elf-gcc compiler to users home directory [Y/n] " yn
    case $yn in
      [Yy]* ) bash ./sync_dependencies.sh; break;;
      [Nn]* ) exit;;
      * ) echo "Please answer yes or no.";;
    esac
  done
fi

if [[ -z "$1" ]]
then
cat << EOF
Chaos Build Scripts
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
EOF
fi

clean() {
  bash ./clean.sh "$1"
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
  bash ./qemu_image.sh "$1"
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

if [ "$1" == "clean" ]
then
  clean "$2"
fi