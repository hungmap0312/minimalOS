; Khai báo các hằng số cho Multiboot header
MBALIGN     equ  1<<0
MEMINFO     equ  1<<1
FLAGS       equ  MBALIGN | MEMINFO
MAGIC       equ  0x1BADB002
CHECKSUM    equ -(MAGIC + FLAGS)

; Phần chuẩn Multiboot header
section .multiboot
align 4
    dd MAGIC
    dd FLAGS
    dd CHECKSUM

; Khởi tạo vùng nhớ Stack cho Kernel (16KB)
section .bss
align 16
stack_bottom:
resb 16384 ; 16 KB
stack_top:

; Mã thực thi chính
section .text
global _start
extern kernel_main

_start:
    ; Thiết lập thanh ghi esp trỏ đến đỉnh của stack
    mov esp, stack_top
    
    ; Đẩy con trỏ chứa thông tin Multiboot vào stack (để truyền tham số cho C nếu cần)
    push ebx 
    
    ; Gọi hàm chính của Kernel (viết bằng C)
    call kernel_main
    
    ; Nếu Kernel thoát ra (return), ta sẽ treo hệ thống an toàn
    cli
.hang:  
    hlt
    jmp .hang
