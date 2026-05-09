global idt_flush

idt_flush:
    mov eax, [esp+4]  ; Lấy tham số (con trỏ idt_ptr) được truyền từ C
    lidt [eax]        ; Nạp IDT mới vào CPU
    ret               ; Trở về hàm C
