#include "../../include/gdt.h"
#include "../../include/console.h"

void kernel_main(void) {
    // Khởi tạo màn hình
    console_init();
    init_gdt();

    // In các thông báo chào mừng có xuống dòng
    console_write("Hello MinimalOS!\n");
    console_write("Console module initialized successfully.\n");
    console_write("Kernel is ready for the next stage!\n");
    console_write("GDT initialized.\n");
}
