CODE_SEL equ 0x0008
DATA_SEL equ 0x0010
SCRN_SEL equ 0x0018

[bits 32]

;##############################################################################
;################################### INIT #####################################
;##############################################################################

segment .text
	global init, _println, _printtb, _print_reg
	extern main

page_dir:
init:
    mov ax, DATA_SEL
    mov ds, ax
    mov ss, ax
    mov esp, init_stack 
    call load_gdt
    call load_idt
    call flush_sreg
    call clear_screen
	jmp setup_page_dir

load_gdt:
    lgdt ds:[gdt_48]
    ret

load_idt:
    mov eax, CODE_SEL                       ;; prepare descriptor
    shl eax, 16
    mov ax, interrupt_ignore                 
    mov ebx, 0x8e00
    mov ecx, 256                            ;; copy to idt  
    mov esi, idt
f_li:
    mov dword ds:[esi], eax
    mov dword ds:[esi+4], ebx
    add esi, 8
    dec ecx
    jnz f_li
    mov eax, CODE_SEL                       ;; prepare syscall_gate descriptor
    shl eax, 16
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

;##############################################################################
;############################## PAGE TABLE ####################################
;##############################################################################

align 0x1000

page_table:
	times 0x10000 db 0

setup_page_dir:
	mov ax, DATA_SEL
	mov ds, ax

	mov esi, page_dir
f_spd_1:
	mov dword ds:[esi], 0
	add esi, 4	
	cmp esi, 0x1000
	jl f_spd_1

	mov ecx, 1
	mov esi, page_dir
f_spd_2:
	mov eax, ecx
	shl eax, 12
	inc eax
	mov ds:[esi], eax
	add esi, 4
	inc ecx
	cmp ecx, 0x10
	jle f_spd_2

setup_page_table:
	mov ecx, 0
	mov esi, page_table
f_spt:
	mov eax, ecx
	shl eax, 12
	inc eax
	mov ds:[esi], eax
	add esi, 4
	inc ecx
	cmp ecx, 0x4000
	jl f_spt

	xor eax, eax
	mov cr3, eax
	mov eax, cr0
	or eax, 0x80000000
	mov cr0, eax

	mov eax, ds:[0x0000]
	call print_reg
	call println
	mov eax, ds:[0x0001]
	call print_reg
	call println
	mov eax, ds:[0x1000]
	call print_reg
	call println
	mov eax, ds:[0x2000]
	call print_reg
	call println
	mov eax, ds:[0x3000]
	call print_reg
	call println
	mov eax, ds:[0xa000]
	call print_reg
	call println
	mov eax, ds:[0xc000]
	call print_reg
	call println
	mov eax, ds:[0x10000]
	call print_reg
	call println
	jmp $
	call main

;##############################################################################
;################################ INTERRUPT ###################################
;##############################################################################

interrupt_ignore:
    iret

sys_call:
    iret

time_interrupt:
    iret

;##############################################################################
;############################### INIT STACK ###################################
;##############################################################################

    times 1024 db 0
init_stack:

;##############################################################################
;############################### IDT & GDT ####################################
;##############################################################################

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
    times 256*8-($-gdt) db 0

;##############################################################################
;################################# PRINT ######################################
;##############################################################################

tty_pos:
    dw 0

_println:
	call println
	ret

_printtb:
	call printtb
	ret

_print_reg:
	push ebp
	mov ebp, esp
	mov eax, [ebp+8]
	call print_reg
	mov esp, ebp
	pop ebp
	ret

println:
    push gs
    push ds
    push ebx
    push eax
    mov ax, SCRN_SEL
    mov gs, ax
    mov ax, DATA_SEL
    mov ds, ax
    mov ax, ds:[tty_pos]
    mov bl, 0x50
    div bl
    add al, 1
    mul bl
    mov ds:[tty_pos], ax
    pop eax
    pop ebx
    pop ds
    pop gs
    ret

printtb:
    push gs
    push ds
    push eax
    mov ax, SCRN_SEL
    mov gs, ax
    mov ax, DATA_SEL
    mov ds, ax
    mov ax, ds:[tty_pos]
    add ax, 4
    mov ds:[tty_pos], ax
    pop eax
    pop ds
    pop gs
    ret

print_char:
    push gs
    push ds
    push esi
    push ebp
    push eax 
    mov ax, SCRN_SEL
    mov gs, ax
    mov ax, DATA_SEL
    mov ds, ax
    mov ebp, esp
    mov si, ds:[tty_pos]
    shl si, 1
    mov al, ds:[ebp+3]
    mov gs:[si], al
    mov al, ds:[ebp+2]
    mov gs:[si+2], al
    mov al, ds:[ebp+1]
    mov gs:[si+4], al
    mov al, ds:[ebp]
    mov gs:[si+6], al
    shr si, 1
    add si, 4
    mov ds:[tty_pos], si
    pop eax
    pop ebp
    pop esi
    pop ds
    pop gs
    ret

clear_screen:
    push gs
    push ax
    push bx
    push cx
    push si
    mov ax, SCRN_SEL
    mov gs, ax
    mov bl, 32
    mov cx, 80 * 25
    mov si, 0
f_cs: 
    mov gs:[si], bl
    add si, 2
    dec cx
    jnz f_cs
    pop si
    pop cx
    pop bx
    pop ax
    pop gs
    ret

print_reg:
    push gs
    push ds
    push esi
    push ebp
    push eax
    mov ax, DATA_SEL
    mov ds, ax
    mov ax, SCRN_SEL
    mov gs, ax
    mov ebp, esp
    mov si, ds:[tty_pos]    
    shl si, 1    
    mov byte al, ds:[ebp+3] 
    call f_pr_1
    add si, 4
    mov byte al, ds:[ebp+2]
    call f_pr_1
    add si, 4
    mov byte al, ds:[ebp+1]
    call f_pr_1
    add si, 4
    mov byte al, ds:[ebp]
    call f_pr_1
    add si, 4
    shr si, 1
    mov ds:[tty_pos], si
    pop eax
    pop ebp
    pop esi
    pop ds
    pop gs
    ret
f_pr_1:
    push ax
    push bx
    push si
    mov bl, al
    and bl, 0x0f
    add si, 2
    call f_pr_2
    mov bl, al
    shr bl, 4
    sub si, 2
    call f_pr_2
    pop si
    pop bx
    pop ax
    ret
f_pr_2:
    push bx
    cmp bl, 0x09
    ja f_pr_3
    add bl, 48
    mov byte gs:[si], bl
    pop bx
    ret
f_pr_3:
    add bl, 55
    mov byte gs:[si], bl
    pop bx
    ret

;##############################################################################
;################################## END #######################################
;##############################################################################
