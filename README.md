# Chaos

Chaos is a simple hobby os developed mainly by curiosity about how operating systems work

## Building and Running
Development is mainly done with Clion as IDE and wsl (Ubuntu-24.04) as os for the cross-compiler

### Building
If you don't have the cross compiling toolchain setup simply call the ```bash ./chaos.sh``` script in the root dir without arguments, 
and it should detect that you don't have the toolchain setup for the current wsl user, and it will prompt you if it 
can install the required packages and download the required sources and build the cross compiling toolchain

After the cross compiling toolchain is set up, and you can run ```bash ./chaos.sh``` without any install prompt then simply run ```bash ./chaos.sh build``` this will build the kernel and the os image

### Running
#### Requirements
If you want to run the os then you need to have qemu installed in windows along with the qemu binaries available in 
PATH (this means you should be able to open a terminal and write qemu-system-i386, and it should start a new qemu 
instance)

After making sure you have qemu installed then simply run ```bash ./chaos.sh run-image```

### Configuring Clion
If you want to compile and develop the os from clion first make sure that you have followed the step to install the 
toolchain from the Building section

After that simply add a new WSL toolchain to CLion and press add environment and locate the i686.env file it should have 
been created at ```~/i686_cross_compiler/i686.env``` after that you have to set the C compiler and the C++ compiler 
these are found in ```~/opt/cross/i686-elf/bin/``` and are called ```i686-efl-gcc``` and ```i686-elf-g++```. After 
specifying these settings you should have a working WSL cross-compiler for i686-elf setup in CLion which can compile Chaos

After configuring the WSL cross-compiler you have to set up the CMAKE profile this should be the same as any CLion 
project simply pick the WSL Cross compiler as toolchain and leave everything else at default settings except the CMake 
options where we have to add the os architecture and platform settings by pasting the following after any existing 
settings make sure theres a space between any previous setting and the following 
options ```-DOS_ARCH=i686 -DOS_PLATFORM=pc```