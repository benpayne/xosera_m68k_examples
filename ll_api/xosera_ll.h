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

typedef struct LayerStateStruct {
    uint16_t  mBaseAddress;
    uint16_t  mBufferSize;
    uint8_t   mColorBase;
    ColorMode mColorMode;
    RectSize  mSize;
    PixelRepeat mHorizontalRepeat;
    PixelRepeat mVerticalRepeat;
    bool        mEnabled;
    bool        mTiled;
} LayerState;

/**
 * configure the graphics subsystem.  Must be called before any other APIs. 
 * 
 * gm - the mode for the display
 */
int initGraphics(GlobalMode gm);

/**
 * allocate the VRAM for the playfield and configure that playfield to display.
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
 * free the VRAM allocated to a playfield and disable it from displaying.
 * 
 * pf - which playfield to free
 */
int freePlayfield(Playfield pf);

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


void wait_vsync();
