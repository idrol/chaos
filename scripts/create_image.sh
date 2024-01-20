#!/bin/bash

bash ./unmount_image.sh

rm -f ./build/chaos.img
dd if=/dev/zero of=./build/chaos.img bs=512 count=262144
echo "n
p
1


a
w
"|fdisk ./build/chaos.img
sudo losetup /dev/loop3 ./build/chaos.img -o 1048576
sudo mkfs.vfat -F 16 -n "Boot" /dev/loop3
sudo losetup -d /dev/loop3
bash ./mount_image.sh
sudo grub-install --boot-directory=/mnt/chaos/boot --modules="normal part_msdos ext2 fat multiboot biosdisk" --no-floppy /dev/loop2
sudo sync
