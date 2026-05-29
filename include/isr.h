#ifndef ISR_H
#define ISR_H

#include <stdint.h>

// Cấu trúc lưu trữ trạng thái CPU khi có ngắt xảy ra
typedef struct {
    uint32_t ds;                                     // Data segment selector
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax; // Pushed bởi pusha
    uint32_t int_no, err_code;                       // Interrupt number và error code
    uint32_t eip, cs, eflags, useresp, ss;           // Pushed tự động bởi CPU
} registers_t;

void fault_handler(registers_t *r);
void irq_handler(registers_t *r);

#endif

