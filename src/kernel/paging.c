#include "../../include/paging.h"
#include "../../include/pmm.h"
#include "../../include/console.h"

// Con trỏ trỏ đến Page Directory
uint32_t *page_directory;

// Các hàm thao tác với thanh ghi Control Register bằng Inline Assembly
static inline void load_page_directory(uint32_t *pd) {
    __asm__ volatile("mov %0, %%cr3" : : "r"(pd));
}

static inline void enable_paging() {
    uint32_t cr0;
    // Đọc thanh ghi CR0 hiện tại
    __asm__ volatile("mov %%cr0, %0" : "=r"(cr0));
    // Set bit thứ 31 (PG - Paging Enable) thành 1
    cr0 |= 0x80000000;
    // Ghi lại vào CR0
    __asm__ volatile("mov %0, %%cr0" : : "r"(cr0));
}

void init_paging(void) {
    // 1. Cấp phát 1 frame (4KB) cho Page Directory
    page_directory = (uint32_t*)pmm_alloc_frame();
    
    // Khởi tạo toàn bộ 1024 mục của Page Directory thành 0 (chưa present)
    for (int i = 0; i < 1024; i++) {
        // Mặc định cho phép Read/Write, nhưng chỉ dành cho Kernel (PAGE_RW = 2)
        page_directory[i] = PAGE_RW; 
    }

    // 2. Cấp phát 1 frame (4KB) cho Page Table đầu tiên (quản lý từ 0MB -> 4MB)
    uint32_t *first_page_table = (uint32_t*)pmm_alloc_frame();

    // 3. Identity Mapping (Ánh xạ 1:1) cho 4MB đầu tiên
    // Chúng ta lặp 1024 lần, mỗi lần ánh xạ 1 trang 4KB
    for (int i = 0; i < 1024; i++) {
        // Địa chỉ vật lý = i * 4096. 
        // OR với các cờ Present và Read/Write
        first_page_table[i] = (i * 4096) | PAGE_PRESENT | PAGE_RW;
    }

    // 4. Đưa Page Table vừa tạo vào mục đầu tiên (index 0) của Page Directory
    // Địa chỉ của first_page_table là địa chỉ vật lý do pmm_alloc_frame trả về
    page_directory[0] = ((uint32_t)first_page_table) | PAGE_PRESENT | PAGE_RW;

    // 5. Nạp địa chỉ của Page Directory vào thanh ghi CR3
    load_page_directory(page_directory);

    // 6. Kích hoạt Paging!
    enable_paging();
}

uint32_t *clone_kernel_directory(void) {
    // Cấp phát 1 frame vật lý mới (4KB) để làm Page Directory cho Task mới
    uint32_t *new_dir = (uint32_t*)pmm_alloc_frame();
    if (!new_dir) return 0;

    // Sao chép toàn bộ 1024 mục từ Page Directory gốc của Nhân
    // Điều này đảm bảo Task mới kế thừa vùng Identity Mapping 4MB đầu tiên của Nhân
    for (int i = 0; i < 1024; i++) {
        new_dir[i] = page_directory[i];
    }

    return new_dir;
}
