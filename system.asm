CODE_SEL equ 0x0008
DATA_SEL equ 0x0010
SCRN_SEL equ 0x0018
TSS0_SEL equ 0x0020
LDT0_SEL equ 0x0028
TASK0_CODE_SEL equ 0x000f
TASK0_DATA_SEL equ 0x0017

[bits 32]

;######################
;######## INIT ########
;######################

start:
    mov ax, DATA_SEL
    mov ds, ax
    mov ss, ax
    mov esp, init_stack 
    call load_gdt
    call load_idt
    call flush_sreg
    call switch_to_task0

load_gdt:
    lgdt ds:[gdt_48]
    ret

load_idt:
    mov ax, DATA_SEL
    mov ds, ax
    mov eax, CODE_SEL                       ;; prepare descriptor
    shl eax, 16
    mov ax, interrupt_ignore                 
    mov ebx, 0x8e00
    mov ecx, 256                            ;; copy to idt  
    mov esi, idt
f_1:
    mov dword ds:[esi], eax
    mov dword ds:[esi+4], ebx
    add esi, 8
    dec ecx
    jnz f_1
    mov eax, CODE_SEL
    shl eax, 16                      ;; prepare syscall_gate descriptor
    mov ax, sys_call
    mov edx, 0x0000ef00 
    mov ecx, 0x80
    lea esi, [idt+ecx*8]
    mov dword ds:[esi], eax                 ;; move to 0x80
    mov dword ds:[esi+4], edx  
    lidt ds:[idt_48]         
    ret

flush_sreg:
    mov ax, DATA_SEL
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    ret

switch_to_task0:
    pushfd 
    and dword [esp], 0xffffbfff
    popfd 
    mov ax, TSS0_SEL
    ltr ax
    mov ax, LDT0_SEL
    lldt ax
    sti
    push dword TASK0_DATA_SEL
    push dword init_stack
    pushfd
    push dword TASK0_CODE_SEL
    push dword task0
    iret

;######################
;####### TASK0 ########
;######################

task0:
    int 0x80

;######################
;##### INTERRUPT ######
;######################

interrupt_ignore:
    iret

sys_call:
    call say_hello
f_2:
    jmp f_2

;######################
;######## TEST ########
;######################

say_hello:
    mov ax, SCRN_SEL
    mov gs, ax
    call flush
    mov byte gs:[0], 'C'
    mov byte gs:[2], 'O'
    mov byte gs:[4], 'S'
    mov byte gs:[6], ' '
    mov byte gs:[8], 'i'
    mov byte gs:[10], 's'
    mov byte gs:[12], ' '
    mov byte gs:[14], 'L'
    mov byte gs:[16], 'o'
    mov byte gs:[18], 'a'
    mov byte gs:[20], 'd'
    mov byte gs:[22], 'i'
    mov byte gs:[24], 'n'
    mov byte gs:[26], 'g'
    mov byte gs:[28], '.'
    mov byte gs:[30], '.'
    mov byte gs:[32], '.'
    ret

flush:
    mov ax, SCRN_SEL
    mov gs, ax
    mov bl, 32
    mov cx, 80 * 25
    mov si, 0
f_3: 
    mov gs:[si], bl
    add si, 2
    dec cx
    jnz f_3
    ret

;######################
;##### INIT_STACK #####
;######################

    times 32 dd 0
init_stack:

;######################
;##### IDT & GDT ######
;######################

idt_48:
    dw 256*8-1
    dd idt

gdt_48:
    dw 256*8-1
    dd gdt

idt:
    times 256 dd 0, 0

gdt:
    dw 0, 0, 0, 0
    dw 0x07ff, 0x0000, 0x9a00, 0x00c0
    dw 0x07ff, 0x0000, 0x9200, 0x00c0
    dw 0x0002, 0x8000, 0x920b, 0x00c0
    dw 0x0068, tss0, 0xe900, 0x0000
    dw 0x0040, ldt0, 0xe200, 0x0000
    times 250 dd 0, 0

;######################
;#### PAGE TABLE ######
;######################


;######################
;####### TASK #########
;######################

    times 4096*15 db 0

tss0:
    dd 0
    dd task0_kernel_stack, DATA_SEL
    dd 0, 0, 0, 0, 0
    dd 0, 0, 0, 0, 0
    dd 0, 0, 0, 0, 0
    dd 0, 0, 0, 0, 0, 0
    dd LDT0_SEL, 0x8000000
    
ldt0:
    dw 0, 0, 0, 0
    dw 0x03ff, 0x0000, 0xfa00, 0x00c0
    dw 0x03ff, 0x0000, 0xf200, 0x00c0

    times 4096-($-tss0) db 0
task0_kernel_stack:

    times 4096*63 db 0 

;######################
;######## END #########
;######################

    times 1024*512-($-$$) db 0
