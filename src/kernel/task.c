#include "../../include/task.h"
#include "../../include/heap.h"
#include "../../include/pmm.h"
#include "../../include/console.h"
#include "../../include/gdt.h"

task_t *current_task = 0;
task_t *ready_queue = 0;
uint32_t next_pid = 1;

// Hàm Assembly từ switch.asm
extern void switch_context(task_t *old, task_t *new);

// Hàm bọc (Wrapper) để khởi chạy một task mới
void task_wrapper(void (*entry_point)()) {
    // Ngắt có thể đang bị tắt do đang ở trong hàm switch, ta cần bật lại
    __asm__ volatile("sti");
    
    // Gọi hàm thực thi chính của Task
    entry_point();
    
    // Nếu task chạy xong (return), ta treo nó lại để tránh CPU chạy linh tinh
    while(1) {
        __asm__ volatile("hlt");
    }
}

void init_tasking() {
    __asm__ volatile("cli");
    
    // Tạo PCB giả cho hàm kernel_main hiện tại đang chạy
    current_task = (task_t*)kmalloc(sizeof(task_t));
    current_task->id = 0;
    current_task->state = STATE_RUNNING;
    current_task->next = current_task; // Trỏ vào chính nó tạo thành vòng
    
    ready_queue = current_task;
    __asm__ volatile("sti");
}

task_t *create_task(void (*entry_point)()) {
    __asm__ volatile("cli");
    task_t *new_task = (task_t*)kmalloc(sizeof(task_t));
    new_task->id = next_pid++;
    new_task->state = STATE_READY;

    // Cấp phát 1 Frame (4KB) làm Stack
    uint32_t stack_addr = pmm_alloc_frame();
    new_task->stack_limit = stack_addr;
    
    // Đỉnh stack (Phát triển từ địa chỉ cao xuống thấp)
    uint32_t *stack = (uint32_t*)(stack_addr + 4096);

    // BẮT ĐẦU LÀM GIẢ STACK FRAME
    // 1. Tham số truyền vào cho task_wrapper (là địa chỉ hàm entry_point)
    *(--stack) = (uint32_t)entry_point;
    
    // 2. Địa chỉ trả về giả của task_wrapper (Không bao giờ dùng tới)
    *(--stack) = 0x0; 

    // 3. Địa chỉ của task_wrapper (Để lệnh 'ret' trong switch_context nhảy vào)
    *(--stack) = (uint32_t)task_wrapper; 

    // 4. Các thanh ghi callee-saved giả để lệnh 'pop' trong switch_context lấy ra
    *(--stack) = 0; // EBP
    *(--stack) = 0; // EBX
    *(--stack) = 0; // ESI
    *(--stack) = 0; // EDI

    // Lưu đỉnh stack giả này vào PCB
    new_task->esp = (uint32_t)stack;

    // Chèn task mới vào cuối hàng đợi vòng (Round-Robin)
    task_t *last = ready_queue;
    while (last->next != ready_queue) {
        last = last->next;
    }
    last->next = new_task;
    new_task->next = ready_queue; // Trỏ vòng về đầu

    __asm__ volatile("sti");
    return new_task;
}

// Hàm Lập lịch (Round-Robin)
void schedule() {
    if (!current_task || !current_task->next) return;
    
    task_t *old_task = current_task;
    task_t *new_task = current_task->next; // Lấy task tiếp theo trong vòng

    if (old_task == new_task) return; // Nếu chỉ có 1 task thì không cần chuyển

    current_task = new_task;
    set_kernel_stack(current_task->stack_limit + 4096);
    switch_context(old_task, new_task);
}
