#include "../../include/isr.h"
#include "../../include/console.h"

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

void fault_handler(registers_t *r) {
    if (r->int_no < 32) {
        console_write("SYSTEM PANIC: ");
        console_write(exception_messages[r->int_no]);
        console_write("\n");
        for (;;); // Treo hệ thống
    }
}
