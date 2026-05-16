global switch_context

; Hàm chuẩn C: void switch_context(task_t *old, task_t *new);
switch_context:
    ; Lấy tham số từ Stack (Chuẩn cdecl: Tham số nằm ở [ESP+4] và [ESP+8])
    mov eax, [esp+4]    ; eax = con trỏ old_task
    mov edx, [esp+8]    ; edx = con trỏ new_task

    ; 1. Lưu các thanh ghi callee-saved (theo chuẩn C) của task hiện tại
    push ebp
    push ebx
    push esi
    push edi

    ; 2. Lưu đỉnh ngăn xếp hiện tại (ESP) vào trường 'esp' của old_task
    ; (Trường 'esp' nằm ở offset 4 trong cấu trúc task_t)
    mov [eax+4], esp

    ; ---------------- BƯỚC NGOẶT (CONTEXT SWITCH) ----------------
    ; 3. Nạp đỉnh ngăn xếp của task mới vào thanh ghi ESP
    mov esp, [edx+4]
    ; Kể từ dòng này, CPU đang sử dụng ngăn xếp của Task mới!
    ; -------------------------------------------------------------

    ; 4. Phục hồi các thanh ghi callee-saved từ ngăn xếp CỦA TASK MỚI
    pop edi
    pop esi
    pop ebx
    pop ebp

    ; 5. Lệnh ret sẽ lấy địa chỉ (EIP) trên đỉnh ngăn xếp mới và nhảy tới đó
    ret
