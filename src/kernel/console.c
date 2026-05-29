#include "../../include/console.h"

// Kích thước chuẩn của màn hình VGA text mode
static const int VGA_WIDTH = 80;
static const int VGA_HEIGHT = 25;
static uint16_t* const VGA_MEMORY = (uint16_t*) 0xB8000;

// Vị trí con trỏ hiện tại
static int terminal_row;
static int terminal_column;
static uint8_t terminal_color;

void console_init(void) {
    terminal_row = 0;
    terminal_column = 0;
    terminal_color = 0x07; // Màu xám nhạt trên nền đen

    // Vòng lặp xóa toàn bộ màn hình bằng ký tự khoảng trắng
    for (int y = 0; y < VGA_HEIGHT; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            const int index = y * VGA_WIDTH + x;
            VGA_MEMORY[index] = (uint16_t) ' ' | (uint16_t) terminal_color << 8;
        }
    }
}

// HÀM MỚI: Xử lý cuộn màn hình khi đầy
static void console_scroll() {
    // 1. Đẩy toàn bộ dữ liệu từ dòng 1->24 lên dòng 0->23
    for (int y = 1; y < VGA_HEIGHT; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            int to_index = (y - 1) * VGA_WIDTH + x;
            int from_index = y * VGA_WIDTH + x;
            VGA_MEMORY[to_index] = VGA_MEMORY[from_index];
        }
    }

    // 2. Xóa trắng dòng cuối cùng (dòng 24)
    for (int x = 0; x < VGA_WIDTH; x++) {
        int index = (VGA_HEIGHT - 1) * VGA_WIDTH + x;
        VGA_MEMORY[index] = (uint16_t) ' ' | (uint16_t) terminal_color << 8;
    }

    // 3. Neo con trỏ lại ở dòng cuối cùng
    terminal_row = VGA_HEIGHT - 1;
}

void console_putchar(char c) {
    // Xử lý ký tự xuống dòng
    if (c == '\n') {
        terminal_column = 0;
        terminal_row++;
        if (terminal_row == VGA_HEIGHT) {
            console_scroll(); // Gọi hàm cuộn thay vì quay về 0
        }
        return;
    }

    // In ký tự ra bộ đệm VGA
    const int index = terminal_row * VGA_WIDTH + terminal_column;
    VGA_MEMORY[index] = (uint16_t) c | (uint16_t) terminal_color << 8;

    // Di chuyển con trỏ sang phải
    if (++terminal_column == VGA_WIDTH) {
        terminal_column = 0;
        terminal_row++;
        if (terminal_row == VGA_HEIGHT) {
            console_scroll(); // Gọi hàm cuộn thay vì quay về 0
        }
    }
}

void console_write(const char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        console_putchar(str[i]);
    }
}

void console_write_hex(uint32_t n) {
    console_write("0x"); // In tiền tố 0x
    
    // Một số 32-bit có 8 chữ số Hex (mỗi chữ số 4 bit)
    for (int i = 28; i >= 0; i -= 4) {
        uint8_t digit = (n >> i) & 0x0F; // Lấy ra 4 bit hiện tại
        
        if (digit < 10) {
            console_putchar(digit + '0'); // Chuyển từ số 0-9 sang ký tự '0'-'9'
        } else {
            console_putchar(digit - 10 + 'A'); // Chuyển từ số 10-15 sang ký tự 'A'-'F'
        }
    }
}