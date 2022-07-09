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

#define ICON_WIDTH 160
#define ICON_HEIGHT 120

static void run_test()
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
    showPlayfield(PF_B);

    size.height = ICON_HEIGHT;
    size.width = ICON_WIDTH;
    GfxBufferState *icon = allocBitmapBuffer(size, COLOR_4BPP);

    if (!load_sd_colors(PF_B, "/atari_icon.pal.raw"))
    {
        dprintf("Failed to load test.pal.raw\n");
        return;
    }
    if (!load_sd_bitmap(icon, "/atari_icon.raw"))
    {
        dprintf("Failed to load test.raw\n");
        return;
    }


    Position p;
    p.x = 0;
    p.y = 0;
    Rect r;
    r.x = 0;
    r.y = 0;
    r.width = 320;
    r.height = 240;
    int xdir = 1;
    int ydir = 1;

    fillBlit(getPlayfieldGfxBuffer(PF_B), &r, 0xF);

    r.width = icon->mSize.width;
    r.height = icon->mSize.height;

    while(1)
    {
        //dprintf("Draw frame at (%d, %d)\n", r.x, r.y);
        copyBlit(getPlayfieldGfxBuffer(PF_B), &p, icon, &r);
        //fillBlit(getPlayfieldGfxBuffer(PF_B), &r, 0);
        wait_vsync();
        r.x = p.x;
        r.y = p.y;
        fillBlit(getPlayfieldGfxBuffer(PF_B), &r, 0xF);
        r.x = 0;
        r.y = 0;
        
        //r.x += xdir;
        //r.y += ydir;
        p.x += xdir;
        p.y += ydir;

        if ( p.x + r.width >= 320 )
            xdir = -1;
        else if ( p.x == 0 )
            xdir = 1;

        if ( p.y + r.height >= 240 )
            ydir = -1;
        else if ( p.y == 0 )
            ydir = 1;

    }
}




void kmain() 
{
  //printf("Hello, world! 😃\r\n");

  run_test();
  // When this function returns, the machine will reboot.
  //
  // In a real program, you probably wouldn't return from 
  // this function!
}