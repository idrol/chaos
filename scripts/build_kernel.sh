#!/bin/bash

source ~/i686_cross_compiler/i686.env

if [ ! -d "./build/cmake-kernel-debug" ]
then
  cmake -DCMAKE_BUILD_TYPE=Debug -G "Ninja" -DOS_ARCH=i686 -DOS_PLATFORM=pc -S ../ -B ./build/cmake-kernel-debug
fi

cd ./build/cmake-kernel-debug || exit

cmake --build ./ --target kernel

cd ../../

cp ./build/cmake-kernel-debug/kernel ./build/drive/boot