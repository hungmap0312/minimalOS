#include "../../include/gdt.h"
#include <string.h> // Để dùng hàm memset (Nếu bạn chưa có string.h, có thể tự viết vòng lặp gán = 0)

struct gdt_entry_struct {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t  base_middle;
    uint8_t  access;
    uint8_t  granularity;
    uint8_t  base_high;
} __attribute__((packed));

struct gdt_ptr_struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

// Tăng lên 6 mục: Null, KCode, KData, UCode, UData, TSS
struct gdt_entry_struct gdt_entries[6]; 
struct gdt_ptr_struct   gdt_ptr;
tss_entry_t             tss_entry; // Biến toàn cục chứa nội dung TSS

extern void gdt_flush(uint32_t);

static void gdt_set_gate(int32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {
    gdt_entries[num].base_low    = (base & 0xFFFF);
    gdt_entries[num].base_middle = (base >> 16) & 0xFF;
    gdt_entries[num].base_high   = (base >> 24) & 0xFF;

    gdt_entries[num].limit_low   = (limit & 0xFFFF);
    gdt_entries[num].granularity = (limit >> 16) & 0x0F;
    gdt_entries[num].granularity |= gran & 0xF0;
    gdt_entries[num].access      = access;
}

// Hàm khởi tạo nội dung cho mục TSS
static void write_tss(int32_t num, uint16_t ss0, uint32_t esp0) {
    // Tính toán địa chỉ và kích thước của struct tss_entry
    uint32_t base = (uint32_t)&tss_entry;
    uint32_t limit = sizeof(tss_entry_t);

    // Thêm Descriptor của TSS vào GDT (mục số num)
    // Access byte: 0xE9 (Present = 1, DPL = 3, Executable = 0, Accessed = 1)
    gdt_set_gate(num, base, limit, 0xE9, 0x00);

    // Khởi tạo toàn bộ TSS bằng 0 thủ công để tránh phụ thuộc thư viện string.h
    uint8_t *tss_ptr = (uint8_t*)&tss_entry;
    for (uint32_t i = 0; i < sizeof(tss_entry_t); i++) {
        tss_ptr[i] = 0;
    }

    // Thiết lập các giá trị cốt lõi
    tss_entry.ss0  = ss0;   // Stack Segment của Kernel (0x10)
    tss_entry.esp0 = esp0;  // Stack Pointer của Kernel

    // Thiết lập I/O Map Base Address (Không dùng, trỏ ra ngoài kích thước TSS)
    tss_entry.iomap_base = sizeof(tss_entry_t);
}

// Hàm được gọi từ bên ngoài để thay đổi Stack khi Context Switch
void set_kernel_stack(uint32_t stack) {
    tss_entry.esp0 = stack;
}

// Hàm Assembly nội tuyến để nạp Task Register (TR)
static inline void tss_flush() {
    // 0x2B là selector của TSS. 
    // Giải thích: Entry số 5 * 8 = 40 (0x28). Cờ RPL = 3 (Quyền User). 0x28 | 3 = 0x2B.
    __asm__ volatile("ltr %%ax" : : "a" (0x2B));
}

void init_gdt(void) {
    gdt_ptr.limit = (sizeof(struct gdt_entry_struct) * 6) - 1;
    gdt_ptr.base  = (uint32_t)&gdt_entries;

    gdt_set_gate(0, 0, 0, 0, 0);                // 0x00: Null segment
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // 0x08: Kernel Code
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // 0x10: Kernel Data
    gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); // 0x18: User Code (DPL 3)
    gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); // 0x20: User Data (DPL 3)
    
    // 0x28: TSS Segment (Selector 0x2B khi dùng)
    // Tạm thời truyền esp0 = 0, Scheduler sẽ cập nhật giá trị này sau
    write_tss(5, 0x10, 0x0); 

    // Nạp GDT mới
    gdt_flush((uint32_t)&gdt_ptr);
    
    // Nạp TSS (Bắt buộc phải gọi sau gdt_flush)
    tss_flush();
}

