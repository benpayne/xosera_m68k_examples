#include <stdio.h>
#include "xosera_ll.h"

#define GFX_BUFFERS_MAX 32

GlobalState gState;
LayerState  gLayers[2];
GfxBufferState gGfxBuffers[GFX_BUFFERS_MAX];

typedef struct free_mem {
    uint16_t start;
    uint32_t len;
} free_mem_t;

#define VRAM_SEGMENTS   16
#define VRAM_MAX_SIZE   (64 * 1024)

free_mem_t vram_free_list[VRAM_SEGMENTS];

/**
 * configure the graphics subsystem.  Must be called before any other APIs. 
 * 
 * gm - the mode for the display
 */
int initGraphics(GlobalMode gm)
{
    int i;
    //int mode = 0;
    gState.mGfxMode = gm;
    gState.mMemBase = 0;
    gLayers[0].mEnabled = 0;
    gLayers[1].mEnabled = 0;

    // init a list of one node with all of VRAM in it.  This shows all mem as free
    for ( i = 0; i < VRAM_SEGMENTS; i++ )
    {
        vram_free_list[i].len = 0;
    }
    vram_free_list[0].start = 0;
    vram_free_list[0].len = VRAM_MAX_SIZE;

    for ( i = 0; i < GFX_BUFFERS_MAX; i++)
    {
        gGfxBuffers[i].mAllocated = false;
    }

    switch(gm)
    {
        case CONFIG_640_480:
            xosera_init(0);
            gState.mDisplaySize.width = 640;
            gState.mDisplaySize.height = 480;
            break;

        case CONFIG_848_480:
            xosera_init(1);
            gState.mDisplaySize.width = 848;
            gState.mDisplaySize.height = 480;
            break;

        default:
            return XERR_BadGraphicsMode;
    }
    return XERR_NoError;
}

static int alloc_vram(uint16_t size, uint16_t *ptr)
{
    int i;

    for ( i = 0; i < VRAM_SEGMENTS; i++ )
    {
        if ( vram_free_list[i].len >= size )
        {
            *ptr = vram_free_list[i].start;
            vram_free_list[i].len -= size;
            vram_free_list[i].start += size;
            return XERR_NoError;
        }
    }

    return XERR_NoMemory;
}

static int free_vram(uint16_t ptr, uint16_t size)
{
    int temp = ptr;
    temp = size;
    temp = XERR_NoError;
    return temp;
}

static LayerState *get_layer(Playfield pf)
{
    switch (pf)
    {
        case PF_A:
            return &gLayers[0];
        case PF_B:
            return &gLayers[1];
        default:
            return NULL;
    }
}


GfxBufferState *allocBitmapBuffer(RectSize size, ColorMode colors)
{
    int line_words = 0;
    uint16_t reg_bpp = 0;
    GfxBufferState *gfx_buf = NULL;
    int i;

    for ( i = 0; i < GFX_BUFFERS_MAX; i++)
    {
        if ( !gGfxBuffers[i].mAllocated )
        {
            gGfxBuffers[i].mAllocated = true;
            gfx_buf = &gGfxBuffers[i];
            break;
        }
    }

    if ( gfx_buf == NULL )
    {
        return NULL;
    }
    
    uint16_t base_offset;
    if ( alloc_vram(line_words * size.height, &base_offset) != XERR_NoError )
    {
        gGfxBuffers[i].mAllocated = false;
        return NULL;
    }

    gfx_buf->mBaseAddress = base_offset;
    gfx_buf->mColorMode = colors;
    gfx_buf->mSize.width = size.width;
    gfx_buf->mSize.height = size.height;

    switch(colors)
    {
        case COLOR_1BPP:
            gfx_buf->mLineWords = (size.width + 15) / 16;
            break;
        case COLOR_4BPP:
            gfx_buf->mLineWords = (size.width + 3) / 4;
            break;
        case COLOR_8BPP:
            gfx_buf->mLineWords = (size.width + 1) / 2;
            break;
    }

    gfx_buf->mBufferSize = gfx_buf->mLineWords * size.height;

    return gfx_buf;    
}


#define GFX_CTRL_BITMAP 0x0040
#define GFX_CTRL_TILED 0x0000
#define GFX_CTRL_DISABLED 0x0080
#define GFX_CTRL_ENABLED 0x0000
#define GFX_CTRL_ENABLE_MASK 0x0080



/**
 * allocate the VRAM for the playfield and configure that playfield to display.
 * 
 * pf - which playfield to setup
 * size - the width and height of the playfield
 * colors - the number of colors supported on the playfield
 */
int allocBitmapPlayfield(Playfield pf, RectSize size, ColorMode colors)
{
    int line_words = 0;
    uint16_t reg_bpp = 0;

    switch(colors)
    {
        case COLOR_1BPP:
            reg_bpp = 0;
            break;
        case COLOR_4BPP:
            reg_bpp = 1;
            break;
        case COLOR_8BPP:
            reg_bpp = 2;
            break;
    }

    int h_factor = gState.mDisplaySize.width / size.width;
    int v_factor = gState.mDisplaySize.height / size.height;
    
    if (h_factor > 4)
        h_factor = 4;
    if (v_factor > 4)
        v_factor = 4;
        
    uint16_t scale = (h_factor - 1) << 2 | (v_factor - 1);

    GfxBufferState *gfx_buf = allocBitmapBuffer(size, colors);

    if ( gfx_buf == NULL )
    {
        return XERR_NoMemory;
    }

    LayerState *layer = get_layer(pf);
    layer->mCtrlReg = GFX_CTRL_DISABLED | GFX_CTRL_BITMAP | reg_bpp << 4 | scale;
    layer->mGfxState = gfx_buf;
    layer->mColorBase = 0;
    layer->mEnabled = 0;
    layer->mHorizontalRepeat = h_factor - 1;
    layer->mVerticalRepeat = v_factor - 1;
    layer->mTiled = false;

    if (pf == PF_A)
    {
        xreg_setw(PA_DISP_ADDR, gfx_buf->mBaseAddress);
        xreg_setw(PA_LINE_LEN, gfx_buf->mLineWords);
        xreg_setw(PA_GFX_CTRL, layer->mCtrlReg);
    }
    else
    {
        xreg_setw(PB_DISP_ADDR, gfx_buf->mBaseAddress);
        xreg_setw(PB_LINE_LEN, gfx_buf->mLineWords);
        xreg_setw(PB_GFX_CTRL, layer->mCtrlReg);
    }
    
    return XERR_NoError;
}

/**
 * TBD
 */
int allocTiledPlayfield(Playfield pf, RectSize size, ColorMode colors)
{
    return XERR_NoError;
}

/**
 * free the VRAM allocated to a playfield and disable it from displaying.
 * 
 * pf - which playfield to free
 */
int freePlayfield(Playfield pf)
{
    LayerState *layer = get_layer(pf);
    free_vram(layer->mGfxState->mBaseAddress, layer->mGfxState->mBufferSize);
    layer->mEnabled = false;
    xreg_setw(PA_GFX_CTRL, GFX_CTRL_DISABLED);
    return XERR_NoError;
}

int showPlayfield(Playfield pf)
{
    LayerState *layer = get_layer(pf);
    layer->mCtrlReg &= ~GFX_CTRL_ENABLE_MASK;
    layer->mCtrlReg |= GFX_CTRL_ENABLED;
    if (pf == PF_A)
    {
        xreg_setw(PA_GFX_CTRL, layer->mCtrlReg);
    }
    else
    {
        xreg_setw(PB_GFX_CTRL, layer->mCtrlReg);
    }
    return XERR_NoError;
}

int hidePlayfield(Playfield pf)
{
    LayerState *layer = get_layer(pf);
    layer->mCtrlReg &= ~GFX_CTRL_ENABLE_MASK;
    layer->mCtrlReg |= GFX_CTRL_DISABLED;
    if (pf == PF_A)
    {
        xreg_setw(PA_GFX_CTRL, layer->mCtrlReg);
    }
    else
    {
        xreg_setw(PB_GFX_CTRL, layer->mCtrlReg);
    }
    return XERR_NoError;
}

/**
 * Write data to the VRAM for a playfield.  Writes will be truncated at the end of the playfield buffer.
 * 
 * pf - playfield to write to
 * buffer - data to write to the playfield
 * size - size of the data to write to the playfield
 * dest_offset - the word offset into the playfield buffer (16bit word)
 */
int bitmapWrite(Playfield pf, uint16_t *buffer, uint16_t size, uint16_t dest_offset )
{
    LayerState *layer = get_layer(pf);
    int cnt   = 0;
    int vaddr = layer->mGfxState->mBaseAddress + dest_offset;

    xm_setw(WR_INCR, 0x0001);        // needed to be set
    xm_setw(WR_ADDR, vaddr);

    uint16_t *maddr = (uint16_t*)buffer;
    for (int i = 0; i < size; i++)
    {
        xm_setw(DATA, *maddr++);
    }
    return XERR_NoError;
}

/**
 * Update the Palette memory for a playfield, each palette entry is one word (16 bits), 4 bits per component in the format of ARGB.  Up to 256 words per playfield.   Writes will be truncated at the end of the palette buffer.
 * 
 * pf - playfield to write to
 * buffer - data to write to the playfield
 * size - size of the data to write to the playfield
 * dest_offset - the word offset into the playfield buffer (16bit word)
 */
int paletteWrite(Playfield pf, uint16_t *buffer, uint16_t size, uint16_t dest_offset )
{
    LayerState *layer = get_layer(pf);
    int cnt   = 0;
    int vaddr = layer->mGfxState->mBaseAddress + dest_offset;

    if ( pf == PF_A )
        xm_setw(XR_ADDR, XR_COLOR_A_ADDR + dest_offset);
    else
        xm_setw(XR_ADDR, XR_COLOR_B_ADDR + dest_offset);

    uint16_t *maddr = (uint16_t*)buffer;
    for (int i = 0; i < size; i++)
    {
        xm_setw(XR_DATA, *maddr++);
    }
    return XERR_NoError;
}

void wait_vsync()
{
    while (xreg_getw(SCANLINE) >= 0x8000)
        ;
    while (xreg_getw(SCANLINE) < 0x8000)
        ;
}
