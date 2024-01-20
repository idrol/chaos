#!/bin/bash

if [ "$1" == "debug" ]
then
  cmd.exe /c "cmd_qemu_image_debug.bat"
else
  cmd.exe /c "cmd_qemu_image.bat"
fi
