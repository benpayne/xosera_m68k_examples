#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <basicio.h>
#include <machine.h>
#include <sdfat.h>

#include "xosera_ll.h"
#include "debug_print.h"
#include "image_loader.h"

void run_test()
{
    initGraphics(CONFIG_640_480);


    if (SD_check_support())
    {
        dprintf("SD card supported: ");

        if (SD_FAT_initialize())
        {
            dprintf("SD card ready\n");
        }
        else
        {
            dprintf("no SD card\n");
        }
    }
    else
    {
        dprintf("No SD card support.\n");
    }

    RectSize size;
    size.width = 320;
    size.height = 240;

    allocBitmapPlayfield(PF_A, size, COLOR_8BPP);
    allocBitmapPlayfield(PF_B, size, COLOR_4BPP);

    dprintf("playfield A %p\n", getPlayfieldGfxBuffer(PF_A));
    dprintf("playfield B %p\n", getPlayfieldGfxBuffer(PF_B));
    dprintf("playfield A base addr %d\n", getPlayfieldGfxBuffer(PF_A)->mBaseAddress);
    dprintf("playfield B base addr %d\n", getPlayfieldGfxBuffer(PF_B)->mBaseAddress);

    if (!load_sd_colors(PF_A, "/test.pal.raw"))
    {
        dprintf("Failed to load test.pal.raw\n");
        return;
    }
    if (!load_sd_bitmap(getPlayfieldGfxBuffer(PF_A), "/test.raw"))
    {
        dprintf("Failed to load test.raw\n");
        return;
    }

    showPlayfield(PF_A);
    delay(100000);

    uint16_t ctrl = xreg_getw(PA_GFX_CTRL);
    dprintf("Showing Playfield A %x\n", ctrl);

    if (!load_sd_colors(PF_B, "/atari_logo.pal.raw"))
    {
        dprintf("Failed to load atari_logo.pal.raw\n");
        return;
    }
    if (!load_sd_bitmap(getPlayfieldGfxBuffer(PF_B), "/atari_logo.raw"))
    {
        dprintf("Failed to load atari_logo.raw\n");
        return;
    }

    ctrl = xreg_getw(PB_GFX_CTRL);
    dprintf("Showing B before Playfield %x\n", ctrl);
    showPlayfield(PF_B);
    ctrl = xreg_getw(PB_GFX_CTRL);
    dprintf("Showing Playfield B %x\n", ctrl);

    while (!checkchar())
    {
        wait_vsync();
    }

    dprintf("loop complete, resettting...\n");
}