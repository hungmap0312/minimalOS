#ifndef GDT_H
#define GDT_H

#include <stdint.h>

// Cấu trúc của TSS (Task State Segment)
struct tss_entry_struct {
    uint32_t prev_tss;   // TSS trước đó (Dùng cho Hardware Task Switching)
    uint32_t esp0;       // Stack pointer khi chuyển từ User Mode sang Kernel Mode
    uint32_t ss0;        // Stack segment khi chuyển sang Kernel Mode
    uint32_t esp1;       // Các trường dưới đây CPU x86 yêu cầu phải có dù không dùng tới
    uint32_t ss1;
    uint32_t esp2;
    uint32_t ss2;
    uint32_t cr3;
    uint32_t eip;
    uint32_t eflags;
    uint32_t eax, ecx, edx, ebx, esp, ebp, esi, edi;
    uint32_t es, cs, ss, ds, fs, gs;
    uint32_t ldt;
    uint16_t trap;
    uint16_t iomap_base;
} __attribute__((packed));

typedef struct tss_entry_struct tss_entry_t;

void init_gdt(void);

// Hàm dùng để cập nhật lại ESP0 mỗi khi Scheduler chuyển đổi Task
void set_kernel_stack(uint32_t stack);

#endif

