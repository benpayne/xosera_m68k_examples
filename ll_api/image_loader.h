#ifndef __IMAGE_LOADER_H_
#define __IMAGE_LOADER_H_

#include "xosera_ll.h"

bool load_sd_bitmap(GfxBufferState *gfx, const char * filename);
bool load_sd_colors(Playfield pf, const char * filename);


#endif // __IMAGE_LOADER_H_