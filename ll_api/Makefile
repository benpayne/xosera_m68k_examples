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
IMAGE_ASSETS = test.raw atari_logo.raw test.pal.raw atari_logo.pal.raw atari_icon.raw atari_icon.pal.raw
EXTRA_CLEAN = $(IMAGE_ASSETS)

include $(TOPDIR)/build/Make.Defaults
include $(TOPDIR)/build/Make.Rules

images: $(IMAGE_ASSETS)

test.raw test.pal.raw : $(TOPDIR)/assets/bugatti-vision-gt.jpg 
	python3 $(TOPDIR)/scripts/convert.py $< $(basename $@) -c 256

atari_logo.raw atari_logo.pal.raw : $(TOPDIR)/assets/art_033117_side01.png
	python3 $(TOPDIR)/scripts/convert.py $< $(basename $@) -c 16

atari_icon.raw atari_icon.pal.raw : $(TOPDIR)/assets/art_033117_side01.png
	python3 $(TOPDIR)/scripts/convert.py $< $(basename $@) -c 16 -x 160 -y 120

copy_images:
	echo $(IMAGE_ASSETS) $(DEPLOY_PATH)