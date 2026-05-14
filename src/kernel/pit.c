#include "../../include/pit.h"
#include "../../include/io.h"

void init_timer(uint32_t frequency) {
    // Tần số gốc của thạch anh trên PIT là 1193180 Hz
    uint32_t divisor = 1193180 / frequency;

    // Gửi byte cấu hình: Kênh 0, chế độ ngắt định kỳ (Mode 3)
    outb(0x43, 0x36);

    // Gửi byte thấp và byte cao của divisor
    outb(0x40, (uint8_t)(divisor & 0xFF));
    outb(0x40, (uint8_t)((divisor >> 8) & 0xFF));
}
