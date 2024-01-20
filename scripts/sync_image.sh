#!/bin/bash

echo "Syncing files from ./build/drive to /mnt/chaos"
sudo cp -r ./build/drive/** /mnt/chaos/
sudo sync