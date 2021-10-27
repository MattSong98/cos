; Note This File should be rewritten to boot.S (GNU)

BOOTSEG equ 0x07c0
INITSEG equ 0x9000
SYSSEG  equ 0x1000
SYSLEN  equ 1024

CODE_SEL equ 0x0008
DATA_SEL equ 0x0010
TMP_SEL equ 0x0018

[bits 16]

    jmp BOOTSEG:go        
go:  
    mov ax, BOOTSEG
    mov bx, INITSEG
    mov cx, 1
    mov ss, ax
    mov sp, 0x400
    call move_block
    jmp INITSEG:continue

continue:
    mov ax, cs            ;; setup stack             
    mov ss, ax             
    mov sp, 0x400
    call load_system      ;; read system to 0x10000 
    cli                   ;; disable dev interrupt
    call move_system      ;; move system to 0x0000
    call en_prot          ;; enable proc mode

load_system:
    mov di, SYSLEN
    mov ax, SYSSEG
    mov es, ax
    xor bx, bx
    mov ax, 0
    mov si, 1
    call read_floppy_sector
    ret

move_system:
    mov dx, SYSLEN
    shr dx, 7
    mov ax, SYSSEG
    xor bx, bx
    mov cx, 128
f_ms:
    call move_block
    add ax, 0x1000
    add bx, 0x1000
    mov cx, 128
    dec dx
    jnz f_ms
    ret 

en_prot:
    mov ax, cs            ;; load gdtr
    mov ds, ax
    lgdt [gdt_48]         
    in al, 0x92           ;; enable a20
    or al, 0b00000010
    out 0x92, al           
    mov eax,cr0           ;; enable protection mode
    or eax,1
    mov cr0,eax           
    jmp TMP_SEL:init     

[bits 32]

init:
		mov ax, DATA_SEL
		mov ds, ax
		mov ss, ax
		mov esp, 0xA0000
		jmp CODE_SEL:0

; ###########################################
; ################# UTILS ###################
; ###########################################

[bits 16]

;; @ax: segment-from
;; @bx: segment-to
;; @cx: block-len [0, 128]  
;; @ret: null
move_block:
    push word ax
    push word bx
    push word cx
    mov ds, ax
    mov es, bx
    shl cx, 8
    xor si, si
    xor di, di
    cld
    rep movsw
    pop word cx
    pop word bx
    pop word ax
    ret

;; @ax: driver
;; @es: pos-segment
;; @bx: pos-bias
;; @si: sector-start [0, 2879]
;; @di: sector-len
;; @ret: null
read_floppy_sector:
    push word es
    push word si
    push word di
    push word ax  
    push word bx  
    mov bp, sp
f_rfs:
    mov dx, ss:[bp+2]
    mov ax, si
    mov bx, 0x24
    div bl
    mov cl, al
    shl cx, 8    
    shr ax, 8    
    mov bx, 0x12
    div bl
    mov dh, al        
    inc ah
    mov cl, ah       
    mov bx, ss:[bp]   
    mov ax, 0x0201    
    int 0x13
    jc f_rfs                 
    mov bx, es
    add bx, 0x20
    mov es, bx
    inc si
    dec di
    jnz f_rfs
    pop word bx
    pop word ax
    pop word di
    pop word si
    pop word es
    ret
          
; ###########################################
; ################## DATA ###################
; ###########################################

gdt_48:
    dw 4*8-1
    dd INITSEG*16+gdt

gdt:
    dw 0, 0, 0, 0
    dw 0xffff, 0x0000, 0x9a00, 0x00cf
    dw 0xffff, 0x0000, 0x9200, 0x00cf
    dw 0xffff, 0x0000, 0x9a09, 0x00cf
 
    times 510-($-$$) db 0   
    dw 0xaa55    

