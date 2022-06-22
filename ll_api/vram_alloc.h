
#include <stdint.h>
#include <stdbool.h>

void init_vram();
int alloc_vram(uint16_t size, uint16_t *ptr);
int free_vram(uint16_t ptr, uint16_t size);