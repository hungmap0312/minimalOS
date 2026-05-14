#include "../../include/pic.h"
#include "../../include/io.h"

#define PIC1_COMMAND 0x20
#define PIC1_DATA    0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA    0xA1

void pic_remap(void) {
    uint8_t a1, a2;

    // Lưu lại mask hiện tại của PIC
    a1 = inb(PIC1_DATA);
    a2 = inb(PIC2_DATA);

    // ICW1: Bắt đầu chuỗi khởi tạo
    outb(PIC1_COMMAND, 0x11);
    outb(PIC2_COMMAND, 0x11);

    // ICW2: Thiết lập lại Offset
    // Chuyển Master PIC (IRQ 0-7) sang vector 32-39 (0x20)
    outb(PIC1_DATA, 0x20); 
    // Chuyển Slave PIC (IRQ 8-15) sang vector 40-47 (0x28)
    outb(PIC2_DATA, 0x28); 

    // ICW3: Thiết lập PIC Master và Slave nhận diện nhau
    outb(PIC1_DATA, 0x04);
    outb(PIC2_DATA, 0x02);

    // ICW4: Đặt chế độ hoạt động cho x86
    outb(PIC1_DATA, 0x01);
    outb(PIC2_DATA, 0x01);

    // Phục hồi lại mask cũ
    outb(PIC1_DATA, a1);
    outb(PIC2_DATA, a2);
}
