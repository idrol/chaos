#!/bin/bash

if [ ! -d "./build" ]
then
  mkdir ./build
fi

if [ ! -d "./build/drive" ]
then
  mkdir ./build/drive
fi

if [ ! -d "./build/drive/boot" ]
then
  mkdir ./build/drive/boot
fi

if [ ! -d "./build/drive/boot/grub" ]
then
  mkdir ./build/drive/boot/grub
fi

cp ../grub.cfg ./build/drive/boot/grub

bash ./build_kernel.sh

