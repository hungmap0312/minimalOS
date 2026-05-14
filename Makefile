CC = gcc
CFLAGS = -m32 -ffreestanding -O2 -Wall -Wextra
AS = nasm
ASFLAGS = -f elf32
LDFLAGS = -m32 -T src/linker.ld -ffreestanding -O2 -nostdlib

all: minimalos.iso

build/boot.o: src/boot/boot.asm
	$(AS) $(ASFLAGS) $< -o $@

build/gdt_flush.o: src/kernel/gdt_flush.asm
	$(AS) $(ASFLAGS) $< -o $@

build/idt_flush.o: src/kernel/idt_flush.asm
	$(AS) $(ASFLAGS) $< -o $@

build/gdt.o: src/kernel/gdt.c
	$(CC) $(CFLAGS) -c $< -o $@

build/idt.o: src/kernel/idt.c
	$(CC) $(CFLAGS) -c $< -o $@

build/console.o: src/kernel/console.c
	$(CC) $(CFLAGS) -c $< -o $@

build/kernel.o: src/kernel/kernel.c
	$(CC) $(CFLAGS) -c $< -o $@

build/interrupt.o: src/kernel/interrupt.asm
	$(AS) $(ASFLAGS) $< -o $@

build/isr.o: src/kernel/isr.c
	$(CC) $(CFLAGS) -c $< -o $@

build/pic.o: src/kernel/pic.c
	$(CC) $(CFLAGS) -c $< -o $@

build/pit.o: src/kernel/pit.c
	$(CC) $(CFLAGS) -c $< -o $@

build/pmm.o: src/kernel/pmm.c
	$(CC) $(CFLAGS) -c $< -o $@

build/minimalos.bin: build/boot.o build/gdt_flush.o build/idt_flush.o build/interrupt.o build/gdt.o build/idt.o build/isr.o build/pic.o build/pit.o build/pmm.o build/console.o build/kernel.o
	$(CC) $(LDFLAGS) -o $@ $^

minimalos.iso: build/minimalos.bin
	cp build/minimalos.bin isodir/boot/
	grub-mkrescue -o minimalos.iso isodir

run: minimalos.iso
	qemu-system-x86_64 -cdrom minimalos.iso

clean:
	rm -f build/*.o build/*.bin minimalos.iso isodir/boot/minimalos.bin

