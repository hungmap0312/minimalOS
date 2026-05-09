#include "../../include/idt.h"

// Cấu trúc của 1 Entry (mục) trong IDT
struct idt_entry_struct {
    uint16_t base_low;             // 16 bit dưới của địa chỉ hàm xử lý ngắt
    uint16_t sel;                  // Selector của Code Segment trong GDT (thường là 0x08)
    uint8_t  always0;              // Luôn bằng 0
    uint8_t  flags;                // Cờ phân quyền (Quyền Ring 0 hay Ring 3...)
    uint16_t base_high;            // 16 bit trên của địa chỉ hàm xử lý ngắt
} __attribute__((packed));

// Con trỏ IDT để nạp vào CPU
struct idt_ptr_struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

struct idt_entry_struct idt_entries[256];
struct idt_ptr_struct   idt_ptr;

// Hàm Assembly nạp IDT
extern void idt_flush(uint32_t);

// Hàm thiết lập 1 mục trong IDT
void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt_entries[num].base_low  = base & 0xFFFF;
    idt_entries[num].base_high = (base >> 16) & 0xFFFF;
    idt_entries[num].sel       = sel;
    idt_entries[num].always0   = 0;
    idt_entries[num].flags     = flags;
}

void init_idt(void) {
    idt_ptr.limit = sizeof(struct idt_entry_struct) * 256 - 1;
    idt_ptr.base  = (uint32_t)&idt_entries;

    // Khởi tạo toàn bộ 256 ngắt với giá trị 0
    // (Chúng ta sẽ gắn các hàm xử lý thật vào đây ở bước sau)
    for (int i = 0; i < 256; i++) {
        idt_set_gate(i, 0, 0, 0);
    }

    // Nạp IDT vào CPU
    idt_flush((uint32_t)&idt_ptr);
}
