#include "../../include/idt.h"
#include "../../include/gdt.h"
#include "../../include/console.h"

void kernel_main(void) {
    // Khởi tạo màn hình
    console_init();
    init_gdt();
    init_idt();
    console_write("IDT initialized.\n");
    console_write("Hello MinimalOS!\n");
    console_write("Console module initialized successfully.\n");
    console_write("Kernel is ready for the next stage!\n");
    console_write("GDT initialized.\n");

    // Gọi ngắt phần mềm để test
    __asm__ volatile("int $0x3");
}
