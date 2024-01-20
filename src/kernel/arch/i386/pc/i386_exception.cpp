//
// Created by Idrol on 14/05/2023.
//
#include "i386_exception.h"
#include "i386_interrupts.h"
#include <stdint.h>
#include <stdio.h>
#include <panic.h>

__attribute__((interrupt)) void i386_divideByZero(intframe_t *iframe) {
    kernel_panic("Divide by zero!!!");
}

__attribute__((interrupt)) void i386_debug(intframe_t *interrupt_frame) {
    kernel_panic("Debug exception called");
}

__attribute__((interrupt)) void i386_nmi(intframe_t *interrupt_frame) {
    kernel_panic("Non maskable interrupt called");
}

__attribute__((interrupt)) void i386_breakpoint(intframe_t *interrupt_frame) {
    kernel_panic("Breakpoint called");
}

__attribute__((interrupt)) void i386_overflow(intframe_t *interrupt_frame) {
    kernel_panic("Overflow exception called");
}

__attribute__((interrupt)) void i386_bound_range(intframe_t *interrupt_frame) {
    kernel_panic("Bound range exceeded");
}

__attribute__((interrupt)) void i386_invalid_opcode(intframe_t *interrupt_frame) {
    kernel_panic("Invalid opcode called");
}

__attribute__((interrupt)) void i386_device_not_available(intframe_t *interrupt_frame) {
    kernel_panic("Device not available called");
}

__attribute__((interrupt)) void i386_double_fault(intframe_t *interrupt_frame, uint32_t error_code) {
    kernel_panic("Double fault!!!!!");
}

__attribute__((interrupt)) void i386_coproccessor_seg_overrun(intframe_t *interrupt_frame) {
    // Should never be called. (Existed when fpu was a seperate processor)
    kernel_panic("Co-processor segment overrun");
}

__attribute__((interrupt)) void i386_invalid_tss(intframe_t *interrupt_frame, uint32_t error_code) {
    kernel_panic("Invalid TSS");
}

__attribute__((interrupt)) void i386_segment_not_present(intframe_t *interrupt_frame, uint32_t error_code) {
    kernel_panic("Segment not present");
}

__attribute__((interrupt)) void i386_stack_segment_fault(intframe_t *interrupt_frame, uint32_t error_code) {
    kernel_panic("Stack segment fault");
}

__attribute__((interrupt)) void i386_gpf(intframe_t *interrupt_frame, uint32_t error_code) {
    kernel_panic("General protection fault");
}

__attribute__((interrupt)) void i386_page_fault(intframe_t *interrupt_frame, uint32_t error_code) {
    //kernel_panic("Page fault");
    uint32_t virtualAddress;
    asm volatile(
            "mov %%cr2, %%eax\n\t"
            "mov %%eax, %0\n\t"
            :"=m" (virtualAddress)
            : /* No input */
            : "%eax"
            );
    kernel_panic("Page fault 0x%xl", virtualAddress);
    //printf("Page fault %#08x\n", virtualAddress);
    // The address that caused this exception is stored in CR2 but we can only access it from assembly
    bool present = error_code & 0x1; // If set the fault occured due to page protection. If not set the fault was cause by non present page
    bool isWriteError = error_code & 0x1 << 1; // Indicates whethe this fault was caused by a write or read operation
    // If i understood the documentation the flags bellow should only be accessed if you are sure about certain states in the cpu and present = true
    //bool isUser = error_code & 0x1 << 2; // The access level the code that caused this error had. This doesn't mean that it is a privilige error if this is true.
    //bool reserved_write = error_code & 0x1 << 3;
    //bool instruction_fetch = error_code & 0x1 << 4;

    //Terminal::writestring("Page Fault: ");
    //printf("Page fault: 0x%lX\n", virtualAddress);
    if(present) {
        //printf("Page fault: %#08x ", virtualAddress);
        // Page protection error
        // TODO add error checking for bit 2..4
        if(isWriteError) {
            //printf("Tried to write to protected page!\n");
        } else {
            //printf("Tried to read from protected page!\n");
        }
        asm volatile("hlt");
    } else {
        /*uint32_t pdeIndex = virtualAddress >> 22;
        uint32_t physicalPage = KernelMemoryManager::physicalPageFrameAllocator.GetNextFreePage();
        KernelMemoryManager::physicalPageFrameAllocator.SetPageUsed(physicalPage);
        PageDirectory* pde = &Paging::activePageDirectory[pdeIndex];
        pde->SetAddress(physicalPage << 22);
        pde->SetPhysical(true);
        pde->SetPresent(true);
        pde->SetWriteEnabled(true);*/
    }
    asm volatile("hlt");
    return;
}

__attribute__((interrupt)) void i386_floating_point_exception(intframe_t *interrupt_frame) {
    kernel_panic("x87 floating point exception");
}

__attribute__((interrupt)) void i386_alignment_check(intframe_t *interrupt_frame, uint32_t error_code) {
    kernel_panic("Alignment check exception");
}

__attribute__((interrupt)) void i386_machine_check(intframe_t *interrupt_frame) {
    kernel_panic("Machine check exception");
}

__attribute__((interrupt)) void i386_simd_fp_exception(intframe_t *interrupt_frame) {
    kernel_panic("SIMD floating point exception");
}

__attribute__((interrupt)) void i386_virt_exception(intframe_t *interrupt_frame) {
    kernel_panic("Virtualization exception");
}

__attribute__((interrupt)) void i386_security_exception(intframe_t *interrupt_frame, uint32_t error_code) {
    kernel_panic("Security exception");
}

__cdecl void i386_exception_set_exception(uint8_t exceptionNum, uint32_t addr) {
    i386_interrupts_set_entry(exceptionNum, addr, 1, INTERRUPT_GATE);
}

__cdecl void i386_exception_init() {
    i386_exception_set_exception(0, (uint32_t)&i386_divideByZero);
    i386_exception_set_exception(1, (uint32_t)&i386_debug);
    i386_exception_set_exception(2, (uint32_t)&i386_nmi);
    i386_exception_set_exception(3, (uint32_t)&i386_breakpoint);
    i386_exception_set_exception(4, (uint32_t)&i386_overflow);
    i386_exception_set_exception(5, (uint32_t)&i386_bound_range);
    i386_exception_set_exception(6, (uint32_t)&i386_invalid_opcode);
    i386_exception_set_exception(7, (uint32_t)&i386_device_not_available);
    i386_exception_set_exception(8, (uint32_t)&i386_double_fault);
    // 9 Used to be Coprocessor segment overrun. But has not been used sinze fpus was built into cpus.
    i386_exception_set_exception(10, (uint32_t)&i386_invalid_tss);
    i386_exception_set_exception(11, (uint32_t)&i386_segment_not_present);
    i386_exception_set_exception(12, (uint32_t)&i386_stack_segment_fault);
    i386_exception_set_exception(13, (uint32_t)&i386_gpf);
    i386_exception_set_exception(14, (uint32_t)&i386_page_fault);
    // 15 reserved
    i386_exception_set_exception(16, (uint32_t)&i386_floating_point_exception);
    i386_exception_set_exception(17, (uint32_t)&i386_alignment_check);
    i386_exception_set_exception(18, (uint32_t)&i386_machine_check);
    i386_exception_set_exception(19, (uint32_t)&i386_simd_fp_exception);
    i386_exception_set_exception(20, (uint32_t)&i386_virt_exception);
    // 21-29 reserved
    i386_exception_set_exception(30, (uint32_t)&i386_security_exception);
    // 31 reserved
}