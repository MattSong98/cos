; Note This File should be rewritten to head.S (GNU)

CODE_SEL equ 0x0008
DATA_SEL equ 0x0010
SCRN_SEL equ 0x0018

[bits 32]

;##############################################################################
;################################### INIT #####################################
;##############################################################################

segment .text
		global init
		extern main

page_dir:
init:
		mov ax, DATA_SEL
		mov ds, ax
		mov ss, ax
		mov esp, init_stack 
		call load_gdt
		call flush_sreg
		jmp setup_page_dir

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

to_C:
		xor eax, eax
		mov cr3, eax
		mov eax, cr0
		or eax, 0x80000000
		mov cr0, eax
		call main
spin:
		jmp spin

;##############################################################################
;############################### INIT STACK ###################################
;##############################################################################

align 4

		times 1024*20 db 0
init_stack:

;##############################################################################
;################################## GDT #######################################
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
;################################## END #######################################
;##############################################################################
