#include "../../include/idt.h"
#include "../../include/gdt.h"
#include "../../include/console.h"
#include "../../include/pic.h"
#include "../../include/pit.h"
#include "../../include/multiboot.h"
#include "../../include/pmm.h"
#include "../../include/paging.h"
#include "../../include/heap.h"
#include "../../include/task.h"

// Task 1: In chữ 'A'
void task1() {
    while(1) {
        console_write("A");
        // Giả lập delay để mắt người kịp nhìn
        for (volatile int i = 0; i < 10000000; i++); 
    }
}

// Task 2: In chữ 'B'
void task2() {
    while(1) {
        console_write("B");
        for (volatile int i = 0; i < 10000000; i++);
    }
}

// Lấy địa chỉ kết thúc kernel từ linker script
extern uint32_t _kernel_end; 

// Kernel main giờ sẽ nhận tham số ebx từ boot.asm
void kernel_main(multiboot_info_t* mbd) {
    console_init();
    init_gdt();
    init_idt();
    pic_remap(); 
    init_timer(50); 

    console_write("IDT, GDT, PIC, PIT initialized.\n");

    // Lấy tổng dung lượng RAM (bộ nhớ dưới 1MB + bộ nhớ trên 1MB)
    uint32_t mem_size_kb = mbd->mem_lower + mbd->mem_upper;
    
    // Ép kiểu _kernel_end để lấy địa chỉ vật lý
    uint32_t kernel_end_ptr = (uint32_t)&_kernel_end;

    // Khởi tạo Quản lý bộ nhớ vật lý
    pmm_init(kernel_end_ptr, mem_size_kb);
    console_write("Physical Memory Manager initialized.\n");
    
    // Khởi tạo Paging (Bộ nhớ ảo)
    init_paging();
    console_write("Paging enabled successfully.\n");
    
    // Khởi tạo Heap
    init_heap();
    console_write("Kernel Heap initialized.\n");
    
    // Bật hệ thống đa nhiệm
    init_tasking();
    console_write("Multitasking initialized.\n");
    
    // Tạo 2 task mới
    create_task(task1);
    create_task(task2);

    __asm__ volatile("sti");

    // TEST: Thử cấp phát 2 block bộ nhớ
    uint32_t block1 = pmm_alloc_frame();
    uint32_t block2 = pmm_alloc_frame();
    
    if (block1 != 0 && block2 != 0) {
        console_write("PMM Test: Allocated 2 frames successfully!\n");
    }
    
    /* // --- TEST PAGE FAULT ---
    console_write("Testing Page Fault...\n");
    
    // Khai báo con trỏ volatile
    volatile uint32_t *bad_ptr = (volatile uint32_t*)0xA0000000;
    
    // Ép CPU ghi dữ liệu vào vùng nhớ chưa được ánh xạ
    *bad_ptr = 0x12345678; */
    
    // --- TEST KERNEL HEAP ---
    uint32_t *ptr1 = (uint32_t *)kmalloc(100);
    uint32_t *ptr2 = (uint32_t *)kmalloc(200);

    if (ptr1 != 0 && ptr2 != 0) {
        console_write("kmalloc test: Allocated successfully!\n");
        *ptr1 = 0xABCD; // Thử ghi dữ liệu
        *ptr2 = 0x1234;
        
        // Giải phóng
        kfree(ptr1);
        kfree(ptr2);
        console_write("kfree test: Freed successfully!\n");
    } else {
        console_write("Heap allocation failed!\n");
    }

    // Task chính (Kernel ban đầu) in chữ 'K'
    while(1) {
        console_write("K");
        for (volatile int i = 0; i < 10000000; i++);
    }
}
