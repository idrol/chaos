set(ISA_C_FLAGS "-std=gnu99 -ffreestanding -O2 -Wall -Wextra -Werror=return-type -Wno-unused-parameter -fno-exceptions -mno-red-zone -mgeneral-regs-only ")
set(ISA_CPP_FLAGS "-std=c++17 -ffreestanding -O2 -Wall -Wextra -Wno-unused-parameter -fno-exceptions -mno-red-zone -mgeneral-regs-only -fno-rtti")
set(ISA_ASM_FLAGS "-ffreestanding -O2 -Wall -Wextra -Wno-unused-parameter -fno-exceptions -mno-red-zone -mgeneral-regs-only -x assembler-with-cpp")
set(ISA_LINKER_FLAGS "-ffreestanding -O2 -nostdlib")
