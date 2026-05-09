#ifndef IDT_H
#define IDT_H

#include <stdint.h>

// Hàm khởi tạo IDT để gọi trong kernel.c
void init_idt(void);

#endif
