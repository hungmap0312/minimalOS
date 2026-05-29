#ifndef CONSOLE_H
#define CONSOLE_H

#include <stdint.h>

// Khởi tạo màn hình (xóa trắng màn hình)
void console_init(void);

// In một ký tự
void console_putchar(char c);

// In một chuỗi ký tự
void console_write(const char* str);

// In một số nguyên 32-bit dưới dạng Hexadecimal
void console_write_hex(uint32_t n);

#endif
