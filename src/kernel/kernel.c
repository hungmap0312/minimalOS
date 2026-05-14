#include "../../include/idt.h"
#include "../../include/gdt.h"
#include "../../include/console.h"
#include "../../include/pic.h"
#include "../../include/pit.h"
#include "../../include/multiboot.h"
#include "../../include/pmm.h"

// Lấy địa chỉ kết thúc kernel từ linker script
extern uint32_t _kernel_end; 

// Kernel main giờ sẽ nhận tham số ebx từ boot.asm
void kernel_main(multiboot_info_t* mbd) {
    console_init();
    init_gdt();
    init_idt();
    pic_remap(); 
    init_timer(100); 

    console_write("IDT, GDT, PIC, PIT initialized.\n");

    // Lấy tổng dung lượng RAM (bộ nhớ dưới 1MB + bộ nhớ trên 1MB)
    uint32_t mem_size_kb = mbd->mem_lower + mbd->mem_upper;
    
    // Ép kiểu _kernel_end để lấy địa chỉ vật lý
    uint32_t kernel_end_ptr = (uint32_t)&_kernel_end;

    // Khởi tạo Quản lý bộ nhớ vật lý
    pmm_init(kernel_end_ptr, mem_size_kb);
    console_write("Physical Memory Manager initialized.\n");

    __asm__ volatile("sti");

    // TEST: Thử cấp phát 2 block bộ nhớ
    uint32_t block1 = pmm_alloc_frame();
    uint32_t block2 = pmm_alloc_frame();
    
    if (block1 != 0 && block2 != 0) {
        console_write("PMM Test: Allocated 2 frames successfully!\n");
    }

    while(1) {
    }
}
