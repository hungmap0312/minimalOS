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

void console_putchar(char c) {
    // Xử lý ký tự xuống dòng
    if (c == '\n') {
        terminal_column = 0;
        if (++terminal_row == VGA_HEIGHT) terminal_row = 0; // Tạm thời quay về dòng đầu nếu đầy
        return;
    }

    // In ký tự ra bộ đệm VGA
    const int index = terminal_row * VGA_WIDTH + terminal_column;
    VGA_MEMORY[index] = (uint16_t) c | (uint16_t) terminal_color << 8;

    // Di chuyển con trỏ sang phải
    if (++terminal_column == VGA_WIDTH) {
        terminal_column = 0;
        if (++terminal_row == VGA_HEIGHT) terminal_row = 0;
    }
}

void console_write(const char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        console_putchar(str[i]);
    }
}
