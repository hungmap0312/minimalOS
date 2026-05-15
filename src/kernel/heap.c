#include "../../include/heap.h"
#include "../../include/pmm.h"
#include "../../include/console.h"

// Cấu trúc Header quản lý mỗi block bộ nhớ (Nằm ngay trước vùng nhớ cấp phát)
typedef struct heap_block {
    size_t size;               // Kích thước của vùng nhớ (không tính header này)
    uint8_t is_free;           // 1 nếu trống, 0 nếu đang được sử dụng
    struct heap_block *next;   // Con trỏ trỏ đến block tiếp theo
} heap_block_t;

// Con trỏ đầu tiên của danh sách liên kết heap
heap_block_t *heap_head = 0;

void init_heap(void) {
    // Xin PMM 1 frame (4096 bytes) để làm vùng nhớ Heap ban đầu
    // (Trong các OS lớn, khi hết 4096 byte này, kmalloc sẽ tự động xin thêm PMM. 
    // Ở OS tối giản, ta tạm dùng 4KB, có thể xin nhiều frame hơn nếu cần)
    uint32_t heap_addr = pmm_alloc_frame();
    
    heap_head = (heap_block_t *)heap_addr;
    heap_head->size = 4096 - sizeof(heap_block_t);
    heap_head->is_free = 1;
    heap_head->next = 0;
}

void *kmalloc(size_t size) {
    heap_block_t *current = heap_head;

    while (current != 0) {
        // Tìm block trống đầu tiên đủ lớn (First-Fit)
        if (current->is_free && current->size >= size) {
            
            // Nếu block này quá lớn, ta cắt nó ra (Split)
            // Điều kiện: phần thừa phải đủ chứa 1 Header mới + ít nhất 4 bytes dữ liệu
            if (current->size >= size + sizeof(heap_block_t) + 4) {
                // Tạo header cho block mới sinh ra từ phần thừa
                heap_block_t *new_block = (heap_block_t *)((uint8_t*)current + sizeof(heap_block_t) + size);
                new_block->size = current->size - size - sizeof(heap_block_t);
                new_block->is_free = 1;
                new_block->next = current->next;
                
                // Cập nhật lại block hiện tại
                current->size = size;
                current->next = new_block;
            }
            
            current->is_free = 0; // Đánh dấu là đã dùng
            // Trả về địa chỉ của vùng dữ liệu (nằm ngay sau Header)
            return (void *)((uint8_t*)current + sizeof(heap_block_t));
        }
        current = current->next;
    }
    
    return 0; // Hết bộ nhớ Heap
}

void kfree(void *ptr) {
    if (!ptr) return;

    // Lùi lại một khoảng bằng sizeof(heap_block_t) để tìm lại Header
    heap_block_t *block = (heap_block_t *)((uint8_t*)ptr - sizeof(heap_block_t));
    block->is_free = 1;
    
    // Gộp các block trống (Coalesce) để chống phân mảnh
    heap_block_t *current = heap_head;
    while (current != 0 && current->next != 0) {
        if (current->is_free && current->next->is_free) {
            // Cộng dồn kích thước: size của block hiện tại + size của Header + size block tiếp theo
            current->size += sizeof(heap_block_t) + current->next->size;
            // Trỏ vượt qua block tiếp theo
            current->next = current->next->next;
        } else {
            current = current->next;
        }
    }
}
