CODE_SEL equ 0x0008
DATA_SEL equ 0x0010
SCRN_SEL equ 0x0018

[bits 32]

;##############################################################################
;################################### INIT #####################################
;##############################################################################

segment .data

a dd 0x01
b dd 0x02


segment .text
	global init
	extern add

page_dir:
init:
    mov ax, DATA_SEL
    mov ds, ax
    mov ss, ax
    mov esp, init_stack 
    call load_gdt
    call flush_sreg
    call clear_screen
test:
	push dword [a]
	push dword [b]
	call add
	call print_reg
	jmp $
	

load_gdt:
    lgdt ds:[gdt_48]
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
;############################### INIT STACK ###################################
;##############################################################################

align 4

    times 1024 db 0
init_stack:

;##############################################################################
;############################### IDT & GDT ####################################
;##############################################################################

gdt_48:
    dw 256*8-1
    dd gdt

align 8

gdt:
    dw 0, 0, 0, 0
    dw 0x07ff, 0x0000, 0x9a00, 0x00c0
    dw 0x07ff, 0x0000, 0x9200, 0x00c0
    dw 0x0002, 0x8000, 0x920b, 0x00c0
    times 256*8-($-gdt) db 0

;##############################################################################
;################################# PRINT ######################################
;##############################################################################

align 4

tty_pos:
    dw 0

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
