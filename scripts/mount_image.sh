#!/bin/bash

sudo losetup /dev/loop2 ./build/chaos.img
sudo losetup /dev/loop3 ./build/chaos.img -o 1048576
if [ ! -d "/mnt/chaos" ]
then
  sudo mkdir /mnt/chaos
  sudo chmod 777 /mnt/chaos
fi
sudo mount /dev/loop3 /mnt/chaos