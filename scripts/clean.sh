#!/bin/bash

bash ./unmount_image.sh

clean_all()
{
  if [ -d "./build" ]
  then
    rm -rf ./build
  fi
}

clean_image() {
  rm ./build/chaos.img
}

clean_code() {
  rm -rf ./build/cmake-kernel-debug
}

if [ "$1" == "image" ]
then 
  clean_image
fi

if [ "$1" == "code" ]
then
  clean_code
fi

if [ -z "$1" ]
then
  clean_all
fi