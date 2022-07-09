TOPDIR = ..

PLATFORMS = rosco_m68k

TARGET_PROGS = ll_api_test blit_test
TARGET_LIBS = ll_api

SRCS_ll_api = vram_alloc.c xosera_ll.c debug_print.c image_loader.c

SRCS_blit_test = blit_test.c
LIBS_blit_test = ll_api

SRCS_ll_api_test = kmain.c ll_api_test.c
LIBS_ll_api_test = ll_api

INSTALL = images
EXTRA_CLEAN = test.raw atari_logo.raw test.pal.raw atari_logo.pal.raw atari_icon.raw atari_icon.pal.raw

include $(TOPDIR)/build/Make.Defaults
include $(TOPDIR)/build/Make.Rules

images: test.raw atari_logo.raw atari_icon.raw

test.raw : $(TOPDIR)/assets/bugatti-vision-gt.jpg 
	python3 $(TOPDIR)/scripts/convert.py $< $(basename $@) -c 256

atari_logo.raw : $(TOPDIR)/assets/art_033117_side01.png
	python3 $(TOPDIR)/scripts/convert.py $< $(basename $@) -c 16

atari_icon.raw : $(TOPDIR)/assets/art_033117_side01.png
	python3 $(TOPDIR)/scripts/convert.py $< $(basename $@) -c 16 -x 160 -y 120
