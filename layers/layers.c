#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <basicio.h>
#include <machine.h>
#include <sdfat.h>

#include "xosera_m68k_api.h"

uint32_t mem_buffer[128 * 1024];

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
static void dprintf(const char * fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vsnprintf(dprint_buff, sizeof(dprint_buff), fmt, args);
    dprint(dprint_buff);
    va_end(args);
}

void wait_vsync()
{
    while (xreg_getw(SCANLINE) >= 0x8000)
        ;
    while (xreg_getw(SCANLINE) < 0x8000)
        ;
}

static bool load_sd_bitmap(const char * filename, uint16_t base_address)
{
    dprintf("Loading bitmap: \"%s\"", filename);
    void * file = fl_fopen(filename, "r");

    if (file != NULL)
    {
        int cnt   = 0;
        int vaddr = base_address;

        xm_setw(WR_INCR, 0x0001);        // needed to be set

        while ((cnt = fl_fread(mem_buffer, 1, 512, file)) > 0)
        {
            if ((vaddr & 0xFFF) == 0)
            {
                dprintf(".");
            }

            uint16_t * maddr = (uint16_t *)mem_buffer;
            xm_setw(WR_ADDR, vaddr);
            for (int i = 0; i < (cnt >> 1); i++)
            {
                xm_setw(DATA, *maddr++);
            }
            vaddr += (cnt >> 1);
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

static bool load_sd_colors(const char * filename, bool color_a)
{
    dprintf("Loading colormap: \"%s\"", filename);
    void * file = fl_fopen(filename, "r");

    if (file != NULL)
    {
        int cnt   = 0;
        int vaddr = 0;

        while ((cnt = fl_fread(mem_buffer, 1, 512, file)) > 0)
        {
            if ((vaddr & 0x7) == 0)
            {
                dprintf(".");
            }

            uint16_t * maddr = (uint16_t *)mem_buffer;
            if ( color_a )
                xm_setw(XR_ADDR, XR_COLOR_A_ADDR);
            else
                xm_setw(XR_ADDR, XR_COLOR_B_ADDR);
            for (int i = 0; i < (cnt >> 1); i++)
            {
                xm_setw(XR_DATA, *maddr++);
            }
            vaddr += (cnt >> 1);
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

void run_test()
{
    xosera_init(0);

    uint16_t version   = xreg_getw(VERSION);
    uint32_t githash   = ((uint32_t)xreg_getw(GITHASH_H) << 16) | (uint32_t)xreg_getw(GITHASH_L);
    uint16_t monwidth  = xreg_getw(VID_HSIZE);
    uint16_t monheight = xreg_getw(VID_VSIZE);
    uint16_t monfreq   = xreg_getw(VID_VFREQ);

    uint16_t gfxctrl  = xreg_getw(PA_GFX_CTRL);
    uint16_t gfxctrlb  = xreg_getw(PB_GFX_CTRL);
    uint16_t tilectrl = xreg_getw(PA_TILE_CTRL);
    uint16_t tilectrlb = xreg_getw(PB_TILE_CTRL);
    uint16_t dispaddr = xreg_getw(PA_DISP_ADDR);
    uint16_t dispaddrb = xreg_getw(PB_DISP_ADDR);
    uint16_t linelen  = xreg_getw(PA_LINE_LEN);
    uint16_t linelenb  = xreg_getw(PB_LINE_LEN);
    uint16_t hvscroll = xreg_getw(PA_HV_SCROLL);

    dprintf("Xosera v%1x.%02x #%08x Features:0x%1x\n", (version >> 8) & 0xf, (version & 0xff), githash, version >> 8);
    dprintf("Monitor Mode: %dx%d@%2x.%02xHz\n", monwidth, monheight, monfreq >> 8, monfreq & 0xff);
    dprintf("\nPlayfield A:\n");
    dprintf("PA_GFX_CTRL : 0x%04x PA_TILE_CTRL: 0x%04x\n", gfxctrl, tilectrl);
    dprintf("PA_DISP_ADDR: 0x%04x PA_LINE_LEN : 0x%04x\n", dispaddr, linelen);
    dprintf("\nPlayfield B:\n");
    dprintf("PB_GFX_CTRL : 0x%04x PB_TILE_CTRL: 0x%04x\n", gfxctrlb, tilectrlb);
    dprintf("PB_DISP_ADDR: 0x%04x PB_LINE_LEN : 0x%04x\n", dispaddrb, linelenb);
    dprintf("PA_HV_SCROLL: 0x%04x\n", hvscroll);



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

    if (!load_sd_colors("/test.pal.raw", true))
    {
        return;
    }

    if (!load_sd_colors("/atari_logo.pal.raw", false))
    {
        return;
    }

    if (!load_sd_bitmap("/test.raw", 0))
    {
        return;
    }

    uint32_t offset = 320 * 240 / 2;

    if (!load_sd_bitmap("/atari_logo.raw", offset))
    {
        return;
    }

    xreg_setw(PB_DISP_ADDR, offset);

    // Set line len here, if the two res had different the copper
    // would handle this instead...
    xreg_setw(PA_LINE_LEN, 160);
    xreg_setw(PB_LINE_LEN, 80);

    /* For manual testing tut, if copper disabled */
    xreg_setw(PA_GFX_CTRL, 0x0065);
    xreg_setw(PB_GFX_CTRL, 0x0055);

    // dprintf("Ready - enabling copper...\n");
    // xreg_setw(COPP_CTRL, 0x8000);

    /* For manual testing mountain if copper disabled... */
    // xreg_setw(PA_GFX_CTRL, 0x0040);
    // xreg_setw(PA_LINE_LEN, 80);
    // xreg_setw(PA_DISP_ADDR, 0x3e80);

    // bool     up      = false;
    // uint16_t current = 240;

    while (!checkchar())
    {
        wait_vsync();
    }

    dprint("loop complete, resettting...\n");

    // disable Copper
    xreg_setw(COPP_CTRL, 0x0000);

    // restore text mode
    xosera_init(1);
    xreg_setw(PA_GFX_CTRL, 0x0000);        // un-blank screen
    print("\033c");                        // reset & clear

}