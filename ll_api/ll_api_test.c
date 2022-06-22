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

uint16_t mem_buffer[1024];

static void dputc(char c)
{
#ifndef __INTELLISENSE__
    __asm__ __volatile__(
        "move.w %[chr],%%d0\n"
        "move.l #2,%%d1\n"        // SENDCHAR
        "trap   #14\n"
        :
        : [chr] "d"(c)
        : "d0", "d1");
#endif
}

static void dprint(const char * str)
{
    register char c;
    while ((c = *str++) != '\0')
    {
        if (c == '\n')
        {
            dputc('\r');
        }
        dputc(c);
    }
}

static char dprint_buff[4096];

extern void dprintf(const char * fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vsnprintf(dprint_buff, sizeof(dprint_buff), fmt, args);
    dprint(dprint_buff);
    va_end(args);
}

static bool load_sd_bitmap(Playfield pf, const char * filename)
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
            bitmapWrite(pf, mem_buffer, cnt >> 1, offset);
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

static bool load_sd_colors(Playfield pf, const char * filename)
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
    if (!load_sd_bitmap(PF_A, "/test.raw"))
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
    if (!load_sd_bitmap(PF_B, "/atari_logo.raw"))
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

    dprint("loop complete, resettting...\n");
}