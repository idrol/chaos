.section .text
.extern currentTask
.extern tss0
.global setGDT
setGDT:
    movl 4(%esp), %eax
    lgdt (%eax)

    movw $0x10, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs
    movw %ax, %ss
    jmp $0x08, $flushGDT
flushGDT:
    ret

.global setIDT
setIDT:
    movl 4(%esp), %eax
    lidt (%eax)
    ret

.global loadPageDirectory
loadPageDirectory:
    push %ebp
    movl %esp, %ebp
    movl 8(%esp), %eax
    movl %eax, %cr3
    movl %ebp, %esp
    pop %ebp
    ret

.global enablePaging
enablePaging:
    push %ebp
    mov %esp, %ebp
    mov %cr0, %eax
    or $0x80000000, %eax
    mov %eax, %cr0
    mov %ebp, %esp
    pop %ebp
    ret

.global invalidatePage
invalidatePage:
    mov %cr3, %eax
    mov %eax, %cr3
    ret

.global flushTSS
flushTSS:
    mov $((5 * 8) | 0), %ax
    ltr %ax
    ret

.macro irq_stub number
    irq\number:
        push $\number
        call i386_irq_handler_common
        add $4, %esp
        iret
.endm

.altmacro

.macro irq_insert number
    .section .text
    irq_stub \number

    .section .data
    .long irq\number
.endm

.section .data
.global default_handlers
default_handlers:
    .set i,0
    .rept 256
        irq_insert %i
        .set i, i+1
    .endr

.section .text

/*.global jumpUserMode
jumpUsermode:
    mov ax, (4 * 8) | 3;
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov eax, esp
    push (4 * 8) | 0x3
    push eax
    pushf
    push (3 * 3) | 0x3
    push test_user_mode
    iret*/

/*.global switch_task
switch_task:
    push %ebx
    push %esi
    push %edi
    push %ebp

    movl $currentTask, %edi // Store address of current task in edi
    movl %esp, (%edi)        // Set current task esp to value of esp

    movl 20(%esp), %esi      // Address of next task struct
    movl (%esi), %esp        // Load esp
    movl 8(%esi), %eax       // Load address of page directory into eax
    movl 4(%esi), %ebx       // Load address of stack top into ebx
    movl $tss0, %edi
    movl %ebx, 4(%edi) // Set new stack top in tss0

    movl %cr3, %ecx          // Only update cr3 (flushing the page directory) if the directories differ
    cmpl %ecx, %eax
    je .doneVASwitch
    movl %eax, %cr3

.doneVASwitch:
    pop %ebp
    pop %edi
    pop %esi
    pop %ebx
    ret*/