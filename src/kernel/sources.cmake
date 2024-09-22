set(KERNEL_SRCS
        ${CMAKE_CURRENT_SOURCE_DIR}/buddy_allocator.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/memory.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/MemoryManager.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/panic.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/partition.c
        ${CMAKE_CURRENT_SOURCE_DIR}/path.c
        ${CMAKE_CURRENT_SOURCE_DIR}/drivers/blockio.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/drivers/device.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/drivers/fat.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/drivers/fs.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/drivers/vfs.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/drivers/thread.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/drivers/interrupts.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/drivers/io.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/drivers/ps2_kb.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/drivers/ata.c
        ${CMAKE_CURRENT_SOURCE_DIR}/sh.cpp
)