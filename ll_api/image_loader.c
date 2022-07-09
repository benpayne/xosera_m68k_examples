
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <basicio.h>
#include <machine.h>
#include <sdfat.h>

#include "debug_print.h"
#include "image_loader.h"

uint16_t mem_buffer[1024];

bool load_sd_bitmap(GfxBufferState *gfx, const char * filename)
{
    dprintf("Loading bitmap: \"%s\"", filename);
    void * file = fl_fopen(filename, "r");

    if (file != NULL)
    {
        int cnt;
        uint16_t offset = 0;

        while ((cnt = fl_fread(mem_buffer, 1, 1024, file)) > 0)
        {
            dprintf(".");
            bitmapWrite(gfx, mem_buffer, cnt >> 1, offset);
            offset += (cnt >> 1);
        }

        fl_fclose(file);
        dprintf("done!\n");
        return true;
    }
    else
    {
        dprintf(" - FAILED\n");
        return false;
    }
}

bool load_sd_colors(Playfield pf, const char * filename)
{
    dprintf("Loading colormap: \"%s\"", filename);
    void * file = fl_fopen(filename, "r");

    if (file != NULL)
    {
        int cnt   = 0;

        if ((cnt = fl_fread(mem_buffer, 1, 512, file)) > 0)
        {
            dprintf(".");
            paletteWrite(pf, mem_buffer, cnt >> 1, 0);
        }
        else
        {
            dprintf(" - FAILED read\n");
            return false;
        }

        fl_fclose(file);
        dprintf("done!\n");
        return true;
    }
    else
    {
        dprintf(" - FAILED open\n");
        return false;
    }
}