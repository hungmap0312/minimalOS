#ifndef HEAP_H
#define HEAP_H

#include <stdint.h>
#include <stddef.h> // Để sử dụng kiểu size_t

void init_heap(void);
void *kmalloc(size_t size);
void kfree(void *ptr);

#endif
