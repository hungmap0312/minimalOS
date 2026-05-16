#ifndef TASK_H
#define TASK_H

#include <stdint.h>

typedef enum {
    STATE_READY,
    STATE_RUNNING
} task_state_t;

typedef struct task {
    uint32_t id;                // Offset 0
    uint32_t esp;               // Offset 4 (Phải khớp với file switch.asm)
    task_state_t state;         // Offset 8
    uint32_t stack_limit;       // Offset 12 (Địa chỉ thấp nhất của stack 4KB)
    uint32_t *page_directory;   // Lưu địa chỉ bảng phân trang riêng
    struct task *next;          // Offset 16 (Cho danh sách liên kết vòng)
} task_t;

void init_tasking(void);
task_t *create_task(void (*entry_point)());
void schedule(void);

#endif
