#ifndef MULTIBOOT_H
#define MULTIBOOT_H

#include <stdint.h>

// Cấu trúc thông tin do GRUB cung cấp theo chuẩn Multiboot 1
typedef struct {
    uint32_t flags;
    uint32_t mem_lower; // Bộ nhớ dưới 1MB (đơn vị KB)
    uint32_t mem_upper; // Bộ nhớ trên 1MB (đơn vị KB)
    // Các trường khác được bỏ qua để giữ tối giản
} __attribute__((packed)) multiboot_info_t;

#endif
