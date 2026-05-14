#ifndef PMM_H
#define PMM_H

#include <stdint.h>

#define PMM_FRAME_SIZE 4096 // Mỗi frame bộ nhớ có kích thước 4KB

// Khởi tạo PMM
void pmm_init(uint32_t kernel_end_addr, uint32_t memory_size);

// Cấp phát 1 frame (trả về địa chỉ vật lý)
uint32_t pmm_alloc_frame(void);

// Giải phóng 1 frame
void pmm_free_frame(uint32_t addr);

#endif
