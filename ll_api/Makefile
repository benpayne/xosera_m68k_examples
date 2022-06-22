# Make rosco_m68k example programs
#
# Copyright (c) 2020 Xark
# MIT LICENSE

TOP=..

# Assume each source files makes an object file
OBJECTS=vram_alloc.o kmain.o ll_api_test.o xosera_ll.o 
EXTRA_CLEAN=atari_logo.raw atari_logo.pal.raw test.raw test.pal.raw
EXTRA_ALL_TARGETS=images

include $(TOP)/defaults.mk

images: test.raw atari_logo.raw

test.raw : $(TOP)/assets/bugatti-vision-gt.jpg 
	python3 $(TOP)/scripts/convert.py $< $(basename $@) 256

atari_logo.raw : $(TOP)/assets/art_033117_side01.png
	python3 $(TOP)/scripts/convert.py $< $(basename $@) 16

# Makefile magic (for "phony" targets that are not real files)
.PHONY: images
