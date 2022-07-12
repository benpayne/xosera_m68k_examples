TOPDIR = .

SUBDIRS = layers ll_api examples

PLATFORM=rosco_m68k
export PLATFORM

include $(TOPDIR)/build/Make.Defaults
include $(TOPDIR)/build/Make.Rules

BINARIES = $(wildcard bin/m68k-elf/*)
DEPLOY_PATH ?= /Volume/ROSCO
export DEPLOY_PATH

deploy:
	echo $(BINARIES) $(DEPLOY_PATH)/
	make -C ll_api copy_images

