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

extern uint32_t _kernel_end;

// ====================================================================
// BẢNG ĐIỀU KHIỂN KỊCH BẢN KIỂM THỬ (TEST SUITE DASHBOARD)
// Thay đổi giá trị của CURRENT_TEST_MODE để chạy các kịch bản khác nhau
// ====================================================================
#define MODE_NORMAL_MULTITASKING  0  // Chế độ chạy Đa nhiệm bình thường (Test 1, 3, 4, 6)
#define MODE_TEST_DIV_BY_ZERO     1  // Kịch bản test lỗi chia cho 0 (Test 2)
#define MODE_TEST_PAGE_FAULT      2  // Kịch bản test truy cập vùng nhớ cấm (Test 5)

// SỬA GIÁ TRỊ Ở ĐÂY ĐỂ ĐỔI BÀI TEST:
#define CURRENT_TEST_MODE         MODE_TEST_PAGE_FAULT
// ====================================================================

// Hàm tạo độ trễ giả để quá trình boot nhìn trực quan hơn
void delay(int loops) {
    for (volatile int i = 0; i < loops * 100000; i++);
}

// ==========================================
// CÁC HÀM DÀNH CHO MULTITASKING TEST
// ==========================================
void task1() {
    while(1) {
        console_write(" [TASK 1] is running smoothly...\n");
        delay(150);
    }
}

void task2() {
    while(1) {
        console_write(" [TASK 2] is running smoothly...\n");
        delay(150);
    }
}

// ==========================================
// HÀM KIỂM TRA RÒ RỈ BỘ NHỚ (MEMORY TEST)
// ==========================================
void run_memory_test() {
    console_write("[i] Running System Memory & Heap Integrity Test...\n");
    delay(50);
    
    int passed = 1;
    for (int i = 0; i < 10000; i++) {
        void *ptr = kmalloc(256);
        if (!ptr) {
            passed = 0;
            break;
        }
        kfree(ptr);
    }

    if (passed) {
        console_write("    -> [ OK ] Allocated & Freed 10,000 blocks successfully.\n");
        console_write("    -> [ OK ] No Memory Leaks detected.\n");
    } else {
        console_write("    -> [FAIL] Out of heap memory! Leak detected.\n");
    }
    delay(100);
}

// ==========================================
// HÀM KHỞI TẠO CHÍNH (KERNEL MAIN)
// ==========================================
void kernel_main(multiboot_info_t* mbd) {
    console_init();
    console_write("==================================================\n");
    console_write("               minimalOS Bootloader               \n");
    console_write("       Authors: Tuan Hung & Quang Huy (HUST)      \n");
    console_write("==================================================\n\n");
    delay(100);

    // 1. KHỞI TẠO CÁC THÀNH PHẦN CỐT LÕI TRƯỚC KHI CHẠY BÀI TEST
    console_write("[*] Booting System Core...\n");
    
    init_gdt();
    console_write("    [ OK ] Global Descriptor Table (GDT) loaded.\n");
    delay(50);

    init_idt();
    console_write("    [ OK ] Interrupt Descriptor Table (IDT) loaded.\n");
    delay(50);

    pic_remap();
    console_write("    [ OK ] Programmable Interrupt Controller (PIC) remapped.\n");
    delay(50);
    
    uint32_t mem_size_kb = mbd->mem_lower + mbd->mem_upper;
    uint32_t kernel_end_ptr = (uint32_t)&_kernel_end;

    pmm_init(kernel_end_ptr, mem_size_kb);
    console_write("    [ OK ] Physical Memory Manager (Bitmap) initialized.\n");
    delay(50);

    init_paging();
    console_write("    [ OK ] Paging & Virtual Memory enabled.\n");
    delay(50);

    init_heap();
    console_write("    [ OK ] Kernel Heap allocated.\n\n");
    delay(100);

    // 2. CHẠY BÀI TEST BỘ NHỚ TRƯỚC KHI BẬT ĐA NHIỆM
    run_memory_test();
    console_write("\n");
    delay(100);

    // 3. XỬ LÝ CÁC KỊCH BẢN TEST THEO YÊU CẦU
    if (CURRENT_TEST_MODE == MODE_TEST_DIV_BY_ZERO) {
        console_write("[-] TRIGGERING TEST: Division By Zero Exception...\n");
        delay(100);
        volatile int a = 10, b = 0;
        volatile int c = a / b; (void)c;
    } 
    else if (CURRENT_TEST_MODE == MODE_TEST_PAGE_FAULT) {
        console_write("[-] TRIGGERING TEST: Page Fault (Illegal Access)...\n");
        delay(100);
        volatile uint32_t *bad_ptr = (volatile uint32_t*)0xA0000000;
        *bad_ptr = 0x1234;
    } 
    else {
        // CHẾ ĐỘ NORMAL MULTITASKING (Mặc định)
        console_write("[*] Initializing Preemptive Multitasking...\n");
        init_timer(50); // Timer 50Hz
        
        init_tasking();
        create_task(task1);
        create_task(task2);

        console_write("    [ OK ] Scheduler Ready. Interrupts Enabled.\n");
        console_write("==================================================\n");
        console_write("         ENTERING MULTITASKING ENVIRONMENT        \n");
        console_write("==================================================\n");
        delay(150);

        __asm__ volatile("sti"); // Bắt đầu kích hoạt Timer và Đa nhiệm!

        while(1) {
            console_write(" [KERNEL] Main thread is idling...\n");
            delay(150);
        }
    }
}