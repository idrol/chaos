#!/bin/bash

if [ -n "$(findmnt -nr -o target -S /dev/loop3)" ]
then
  echo "Unmounting /dev/loop3"
  sudo umount /dev/loop3
fi

if [ -n "$(losetup /dev/loop2)" ]
then
  echo "Detaching /dev/loop2"
  sudo losetup -d /dev/loop2
fi

if [ -n "$(losetup /dev/loop3)" ]
then
  echo "Detaching /dev/loop3"
  sudo losetup -d /dev/loop3
fi
