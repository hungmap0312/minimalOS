/* Chỉ định nhãn _start (trong boot.asm) là điểm bắt đầu (Entry point) */
ENTRY(_start)

SECTIONS
{
    /* Kernel sẽ được GRUB nạp vào địa chỉ bộ nhớ 1 Megabyte */
    . = 1M;

    /* Phần Multiboot header bắt buộc phải nằm ở những byte đầu tiên của kernel */
    .text BLOCK(4K) : ALIGN(4K)
    {
        *(.multiboot)
        *(.text)
    }

    /* Vùng chứa dữ liệu chỉ đọc (ví dụ: các hằng chuỗi văn bản) */
    .rodata BLOCK(4K) : ALIGN(4K)
    {
        *(.rodata)
    }

    /* Vùng chứa các biến toàn cục đã khởi tạo giá trị */
    .data BLOCK(4K) : ALIGN(4K)
    {
        *(.data)
    }

    /* Vùng chứa biến chưa khởi tạo và Stack */
    .bss BLOCK(4K) : ALIGN(4K)
    {
        *(COMMON)
        *(.bss)
    }
}
