BOOTSEG equ 0x07c0
SYSSEG  equ 0x1000
SYSLEN  equ 17

CODE_SEL equ 0x0008
DATA_SEL equ 0x0010

boot:
    jmp BOOTSEG:go        ;; set cs:0x07c0
go: 
    mov ax, cs            ;; setup ds
    mov ds, ax              
    mov ss, ax            ;; setup ss:sp 
    mov sp, 0x400               
    call load_system      ;; read system to 0x10000                
    call move_system      ;; move system to 0x0000
    call en_prot          ;; enable proc mode

load_system:
    mov dh, 0x00          ;; head
    mov dl, 0x00          ;; driver
    mov cx, 0x0002        ;; track(8:2MSB) & section(6)
    mov ax, SYSSEG        ;; pos(es:bx)
    mov es, ax            
    xor bx, bx            
    mov ah, 0x02          ;; sub function
    mov al, SYSLEN        ;; sections to read
    int 0x13
    jnc f_2
f_1:
    call f_1
f_2:    
    ret

move_system:
    mov ax, SYSSEG
    mov ds, ax            ;; ds:si -> 0x1000:0
    xor si, si
    xor ax, ax
    mov es, ax            ;; es:di -> 0x0000:0
    xor di, di
    mov cx, 0x1000        ;; move 4K words = 8KB 
f_3:
    mov ax, ds:[si]
    mov es:[di], ax
    add si, 2
    add di, 2
    dec cx
    jnz f_3
    ret  

en_prot:
    mov ax, BOOTSEG       ;; load gdtr
    mov ds, ax
    lgdt [gdt_48]         
    in al, 0x92           ;; enable a20
    or al, 0b00000010
    out 0x92, al           
    cli                   ;; disable interrupt
    mov eax,cr0           ;; enable protection mode
    or eax,1
    mov cr0,eax           
    jmp CODE_SEL:0        ;; here comes the stranger
    ret

gdt_48:
    dw 0xffff
    dd 0x7c00 + gdt

gdt:
    dw 0, 0, 0, 0
    dw 0x07ff, 0x0000, 0x9a00, 0x00c0
    dw 0x07ff, 0x0000, 0x9200, 0x00c0
    dw 0x0002, 0x8000, 0x920b, 0x00c0

 
    times 510-($-$$) db 0   
    dw 0xaa55              
