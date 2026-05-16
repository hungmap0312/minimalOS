#include "../../include/isr.h"
#include "../../include/console.h"
#include "../../include/pic.h"
#include "../../include/task.h"

char *exception_messages[] = {
    "Division By Zero", "Debug", "Non Maskable Interrupt", "Breakpoint",
    "Into Detected Overflow", "Out of Bounds", "Invalid Opcode", "No Coprocessor",
    "Double Fault", "Coprocessor Segment Overrun", "Bad TSS", "Segment Not Present",
    "Stack Fault", "General Protection Fault", "Page Fault", "Unknown Interrupt",
    "Coprocessor Fault", "Alignment Check", "Machine Check", "Reserved",
    "Reserved", "Reserved", "Reserved", "Reserved",
    "Reserved", "Reserved", "Reserved", "Reserved",
    "Reserved", "Reserved", "Reserved", "Reserved"
};

// àm lấy giá trị CR2
static inline uint32_t get_faulting_address() {
    uint32_t cr2;
    __asm__ volatile("mov %%cr2, %0" : "=r" (cr2));
    return cr2;
}

void fault_handler(registers_t *r) {
    if (r->int_no < 32) {
        console_write("SYSTEM PANIC: ");
        console_write(exception_messages[r->int_no]);
        console_write("\n");
        
        // Nếu là Page Fault (Ngắt số 14)
        if (r->int_no == 14) {
            uint32_t fault_addr = get_faulting_address();
            (void)fault_addr;
            console_write("Faulting Address: ");
            // Tạm thời chưa có hàm printf(%x), nếu bạn đã viết hàm in số Hex 
            // thì hãy in biến fault_addr ra đây để biết chính xác địa chỉ nào gây lỗi.
        }
        
        for (;;); // Treo hệ thống
    }
}

// Biến đếm số lần timer tick
uint32_t tick = 0;

void irq_handler(registers_t *r) {
    if (r->int_no == 32) { // Ngắt Timer (IRQ0)
        // Báo cho PIC biết ngắt đã xử lý xong TRƯỚC KHI chuyển task
        pic_send_eoi(0); 
        
        // Gọi Scheduler để đổi sang task khác
        schedule(); 
        return;
    }
    
    pic_send_eoi(r->int_no - 32);
}

