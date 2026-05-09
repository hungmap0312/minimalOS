global gdt_flush

gdt_flush:
    mov eax, [esp+4]  ; Lấy con trỏ gdt_ptr từ tham số của hàm C
    lgdt [eax]        ; Nạp GDT mới vào CPU

    mov ax, 0x10      ; 0x10 là offset của Data Segment trong GDT (mục số 2)
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    jmp 0x08:.flush   ; 0x08 là offset của Code Segment. Far jump để cập nhật thanh ghi CS
.flush:
    ret
