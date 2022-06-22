#include <stdio.h>
#include "vram_alloc.h"
#include "xosera_ll.h"

typedef struct free_mem {
    uint32_t start;
    uint32_t len;
    bool     used;
    struct free_mem *next;
} free_mem_t;

#define VRAM_SEGMENTS   16
#define VRAM_MAX_SIZE   (64 * 1024)

free_mem_t vram_free_blocks[VRAM_SEGMENTS];
free_mem_t *vram_free_list = NULL;

extern void dprintf(const char * fmt, ...);

static void dump_free_list()
{
    free_mem_t *n = vram_free_list;
    while ( n != NULL )    
    {
        dprintf("Node start %lu, len %lu, next %p\n", n->start, n->len, (void*)n->next);
        n = n->next;
    }
}

void init_vram()
{
    // init a list of one node with all of VRAM in it.  This shows all mem as free
    for ( int i = 0; i < VRAM_SEGMENTS; i++ )
    {
        vram_free_blocks[i].used = false;
    }
    vram_free_blocks[0].start = 0;
    vram_free_blocks[0].len = VRAM_MAX_SIZE;
    vram_free_blocks[0].next = NULL;
    vram_free_blocks[0].used = true;
    vram_free_list = &vram_free_blocks[0];
}

static free_mem_t *get_free_block()
{
    for ( int i = 0; i < VRAM_SEGMENTS; i++ )
    {
        if ( vram_free_blocks[i].used == false )
        {
            vram_free_blocks[i].used = true;
            return &vram_free_blocks[i];
        } 
    }
    
    return NULL;
}

int alloc_vram(uint16_t size, uint16_t *ptr)
{
    int i;
    free_mem_t *n = vram_free_list;
    
    dump_free_list();

    do {
        if ( n->len >= size )
        {
            *ptr = n->start;
            n->len -= size;
            n->start += size;
            dprintf("alloc %u at %u\n", size, *ptr);
            return XERR_NoError;
        }
    } while ( n->next != NULL );

    return XERR_NoMemory;
}

int free_vram(uint16_t ptr, uint16_t size)
{
    free_mem_t *b = get_free_block();
    b->len = size;
    b->start = ptr;
    b->next = NULL;
    free_mem_t *n = vram_free_list;

    // if first block is after us, insert block at front
    if ( ptr < n->start )
    {
        b->next = n;
        vram_free_list = b;
    }
    else
    {
        bool insert = false;
        free_mem_t *prev = n;
        n = n->next;    
        while ( n != NULL )
        {
            // insert in the middle
            if ( ptr < n->start )
            {
                prev->next = b;
                b->next = n;
                insert = true;
                break;
            }
            prev = n;
            n = n->next;
        }
        
        // if not inserted but at end.
        if ( insert == false )
        {
            prev->next = b;
        }
    }
    
    printf("coaless adjacent blocks\n");
    dump_free_list();
    
    // now combined adjacent vram_free_blocks
    free_mem_t *prev = vram_free_list;
    n = prev->next;
    while ( n != NULL )
    {
        //printf("check prev %u:%u, node %u:%u\n", prev->start, prev->len, n->start, n->len);
        // if adjacent
        if ( prev->start + prev->len >= n->start )
        {
            prev->len += n->len - (prev->start + prev->len - n->start);
            prev->next = n->next;
            // free the node
            n->used = false;
            // keep current prev and compare to next node
            n = n->next;
        }
        else
        {
            prev = n;
            n = n->next;
        }
    }
    return XERR_NoError;
}

#if 0 
int main()
{
    init_vram();

    uint16_t addr = 0;
    int res = alloc_vram(16*1024, &addr);
    printf("Addr %u, Res %d\n", addr, res);
    res = alloc_vram(16*1024, &addr);
    printf("Addr %u, Res %d\n", addr, res);
    res = alloc_vram(16*1024, &addr);
    printf("Addr %u, Res %d\n", addr, res);
    res = alloc_vram(16*1024, &addr);
    printf("Addr %u, Res %d\n", addr, res);
    res = alloc_vram(16*1024, &addr);
    printf("Addr %u, Res %d\n", addr, res);

    dump_free_list();
    
    res = free_vram(16*1024, 16*1024);
    printf("Free Res %d\n", res);

    dump_free_list();

    res = free_vram(0, 16*1024);
    printf("Free Res %d\n", res);

    dump_free_list();
    
    res = alloc_vram(16*1024, &addr);
    printf("Addr %u, Res %d\n", addr, res);

    dump_free_list();
    
    return 0;
}

#endif