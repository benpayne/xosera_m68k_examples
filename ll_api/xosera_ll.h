/* Low Level API for Xosera HW */

#include "xosera_m68k_api.h"

typedef enum XoseraLLErrorEnum {
    XERR_NoError = 0,
    XERR_BadGraphicsMode,
    XERR_NoMemory
} XoseraLLError;

typedef enum ColorModeEnum {
    COLOR_1BPP,
    COLOR_4BPP,
    COLOR_8BPP
} ColorMode;

typedef struct PositionStruct {
    uint16_t x;
    uint16_t y;
} Position;

typedef struct RectSizeStruct {
    uint16_t width;
    uint16_t height;
} RectSize;

typedef struct RectStruct {
    uint16_t x;
    uint16_t y;
    uint16_t width;
    uint16_t height;
} Rect;

typedef enum PixelRepeatEnum {
    PR_1X,
    PR_2X,
    PR_3X,
    PR_4X
} PixelRepeat;

typedef enum PlayfieldEnum {
    PF_A,
    PF_B,
} Playfield;

typedef enum GlobalModeEnum {
    CONFIG_640_480,
    CONFIG_848_480
} GlobalMode;

typedef struct GlobalStateStruct {
    GlobalMode mGfxMode;
    uint16_t   mMemBase;
    RectSize   mDisplaySize;
} GlobalState;

typedef struct GfxBufferStateStruct {
    uint16_t  mBaseAddress;
    uint16_t  mBufferSize;
    uint16_t  mLineWords;
    ColorMode mColorMode;
    RectSize  mSize;
    bool      mAllocated;
} GfxBufferState;

typedef struct LayerStateStruct {
    GfxBufferState *mGfxState;
    uint8_t   mColorBase;
    PixelRepeat mHorizontalRepeat;
    PixelRepeat mVerticalRepeat;
    bool        mEnabled;
    bool        mTiled;
    uint16_t    mCtrlReg;
} LayerState;


/**
 * configure the graphics subsystem.  Must be called before any other APIs. 
 * 
 * gm - the mode for the display
 */
int initGraphics(GlobalMode gm);

/**
 * allocate a bitmap buffer of a specified size.  This buffer can be used as a 
 *  source or dest for blits.  
 */
GfxBufferState *allocBitmapBuffer(RectSize size, ColorMode colors);

/**
 * allocate the VRAM for the playfield and configure that playfield to display.  Playfields will
 * start as hidden and you must call showPlayfield for it to become visable.
 * 
 * pf - which playfield to setup
 * size - the width and height of the playfield
 * colors - the number of colors supported on the playfield
 */
int allocBitmapPlayfield(Playfield pf, RectSize size, ColorMode colors);

/**
 * TBD
 */
int allocTiledPlayfield(Playfield pf, RectSize size, ColorMode colors);

/**
 * Get the graphics buffer for a playfield.  This allows you to use the playfield 
 * a part of blitter operations.  
 */
GfxBufferState *getPlayfieldGfxBuffer(Playfield pf);

/**
 * free the VRAM allocated to a playfield and disable it from displaying.
 * 
 * pf - which playfield to free
 */
int freePlayfield(Playfield pf);

/**
 * show the playfield on the display
 */
int showPlayfield(Playfield pf);

/**
 * hide the playfield on the display
 */
int hidePlayfield(Playfield pf);

/**
 * Write data to the VRAM for a playfield.  Writes will be truncated at the end of the playfield buffer.
 * 
 * pf - playfield to write to
 * buffer - data to write to the playfield
 * size - size of the data to write to the playfield
 * dest_offset - the word offset into the playfield buffer (16bit word)
 */
int bitmapWrite(Playfield pf, uint16_t *buffer, uint16_t size, uint16_t dest_offset );

/**
 * Update the Palette memory for a playfield, each palette entry is one word (16 bits), 4 bits per component in the format of ARGB.  Up to 256 words per playfield.   Writes will be truncated at the end of the palette buffer.
 * 
 * pf - playfield to write to
 * buffer - data to write to the playfield
 * size - size of the data to write to the playfield
 * dest_offset - the word offset into the playfield buffer (16bit word)
 */
int paletteWrite(Playfield pf, uint16_t *buffer, uint16_t size, uint16_t dest_offset );


int copyBlit(GfxBufferState dest, Rect dest_rect, GfxBufferState src, Rect src_rect);


void wait_vsync();
