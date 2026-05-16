#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>

// Các cờ (Flags) cơ bản cho Page Directory và Page Table
#define PAGE_PRESENT  0x01   // Bit 0: Trang có tồn tại trong bộ nhớ không?
#define PAGE_RW       0x02   // Bit 1: Có cho phép Read/Write không? (0 = Read Only)
#define PAGE_USER     0x04   // Bit 2: Quyền User hay Kernel? (0 = Kernel)

void init_paging(void);

#endif

