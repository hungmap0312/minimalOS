#include "../../include/pmm.h"
#include "../../include/console.h"

// Quản lý tối đa 4GB RAM, mỗi frame 4KB -> Tối đa 1.048.576 frames
// Mỗi bit trong bitmap đại diện cho 1 frame (1 = đã dùng, 0 = trống)
// Mảng bitmap cần: 1.048.576 / 32 = 32.768 phần tử uint32_t
#define MAX_FRAMES 1048576
uint32_t pmm_bitmap[MAX_FRAMES / 32];

uint32_t pmm_total_frames = 0;
uint32_t pmm_used_frames  = 0;

// Các hàm thao tác bit cơ bản
static inline void bitmap_set(uint32_t bit) {
    pmm_bitmap[bit / 32] |= (1 << (bit % 32));
}

static inline void bitmap_unset(uint32_t bit) {
    pmm_bitmap[bit / 32] &= ~(1 << (bit % 32));
}

static inline int bitmap_test(uint32_t bit) {
    return pmm_bitmap[bit / 32] & (1 << (bit % 32));
}

// Khởi tạo PMM
void pmm_init(uint32_t kernel_end_addr, uint32_t memory_size_kb) {
    // Tính tổng số frame dựa trên tổng RAM (tính bằng Byte)
    pmm_total_frames = (memory_size_kb * 1024) / PMM_FRAME_SIZE;
    
    // Khởi tạo toàn bộ RAM là ĐÃ BỊ CHIẾM DỤNG (để an toàn)
    for (uint32_t i = 0; i < pmm_total_frames / 32; i++) {
        pmm_bitmap[i] = 0xFFFFFFFF; 
    }
    
    // Tính xem Kernel kết thúc ở frame số mấy
    uint32_t kernel_end_frame = (kernel_end_addr / PMM_FRAME_SIZE) + 1;
    
    // Mở khóa (đánh dấu là TRỐNG = 0) cho các frame nằm SAU kernel
    for (uint32_t i = kernel_end_frame; i < pmm_total_frames; i++) {
        bitmap_unset(i);
    }
    
    pmm_used_frames = kernel_end_frame;
}

// Cấp phát 1 frame 4KB
uint32_t pmm_alloc_frame(void) {
    for (uint32_t i = 0; i < pmm_total_frames / 32; i++) {
        if (pmm_bitmap[i] != 0xFFFFFFFF) { // Nếu có ít nhất 1 bit 0 trong khối này
            for (int j = 0; j < 32; j++) {
                int bit = i * 32 + j;
                if (!bitmap_test(bit)) { // Tìm thấy frame trống
                    bitmap_set(bit);     // Đánh dấu là đã dùng
                    pmm_used_frames++;
                    return bit * PMM_FRAME_SIZE; // Trả về địa chỉ vật lý
                }
            }
        }
    }
    return 0; // Hết RAM
}

// Giải phóng 1 frame
void pmm_free_frame(uint32_t addr) {
    uint32_t frame = addr / PMM_FRAME_SIZE;
    if (bitmap_test(frame)) {
        bitmap_unset(frame);
        pmm_used_frames--;
    }
}
