option(OS_ARCH "OS Processor architecture" "i686")

set(CMAKE_C_COMPILER ${OS_ARCH}-elf-gcc)
set(CMAKE_CXX_COMPILER ${OS_ARCH}-elf-g++)
set(CMAKE_ASM-ATT_COMPILER ${OS_ARCH}-elf-g++)

set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
cmake_minimum_required(VERSION 3.13)
project(Chaos)



ENABLE_LANGUAGE(ASM-ATT)
ENABLE_LANGUAGE(CXX)

set(CMAKE_VERBOSE_MAKEFILE TRUE)

ADD_SUBDIRECTORY(src/kernel)