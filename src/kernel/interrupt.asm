%macro ISR_NOERRCODE 1
  global isr%1
  isr%1:
    cli
    push byte 0
    push byte %1
    jmp isr_common_stub
%endmacro

%macro ISR_ERRCODE 1
  global isr%1
  isr%1:
    cli
    push byte %1
    jmp isr_common_stub
%endmacro

; Định nghĩa 32 exceptions cơ bản của CPU x86
ISR_NOERRCODE 0
ISR_NOERRCODE 1
ISR_NOERRCODE 2
ISR_NOERRCODE 3
ISR_NOERRCODE 4
ISR_NOERRCODE 5
ISR_NOERRCODE 6
ISR_NOERRCODE 7
ISR_ERRCODE   8
ISR_NOERRCODE 9
ISR_ERRCODE   10
ISR_ERRCODE   11
ISR_ERRCODE   12
ISR_ERRCODE   13
ISR_ERRCODE   14
ISR_NOERRCODE 15
ISR_NOERRCODE 16
ISR_ERRCODE   17
ISR_NOERRCODE 18
ISR_NOERRCODE 19
ISR_NOERRCODE 20
ISR_NOERRCODE 21
ISR_NOERRCODE 22
ISR_NOERRCODE 23
ISR_NOERRCODE 24
ISR_NOERRCODE 25
ISR_NOERRCODE 26
ISR_NOERRCODE 27
ISR_NOERRCODE 28
ISR_NOERRCODE 29
ISR_ERRCODE   30
ISR_NOERRCODE 31

extern fault_handler

isr_common_stub:
    pusha           ; Lưu các thanh ghi edi, esi, ebp, esp, ebx, edx, ecx, eax
    mov ax, ds      ; Lưu Data Segment hiện tại
    push eax

    mov ax, 0x10    ; Load Kernel Data Segment (Offset 0x10 trong GDT)
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    call fault_handler

    pop eax         ; Phục hồi Data Segment cũ
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    popa            ; Phục hồi các thanh ghi

    add esp, 8      ; Dọn dẹp error code và ISR number đã push ở trên
    sti
    iret            ; Trở về từ ngắt
