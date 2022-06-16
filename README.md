# xosera_m68k_examples

These examples assume that the rosco_m68k repo and the Xosera repo are next to this repo, but this can be overridden with the following variables:

`ROSCO_M68K_DIR - Location for Rosco dir (this is the same as the examples in Rosco's dir)`
[ROSCO_M68K_DIR](https://github.com/rosco-m68k/rosco_m68k)

`XOSERA_M68K_DIR - Location of the Xosera dir`
[XOSERA_M68K_DIR](https://github.com/XarkLabs/Xosera)

With that complete you should be able to just run make in the example directories and build the code (included dependacies like images).  

You will need to have built the `libs` in rosco_m68k and the xosera_m68k_api in Xosera. 

[libs docs](https://github.com/rosco-m68k/rosco_m68k/tree/develop/code/software)

Building the Xosera API can be done by running `make m68k` at the top level of the Xosera repo.

Once you've built the code you will need to copy this to a sd card to run on the target system.  The copy.sh script can be used help this.  
This script is designed for working on MacOS and uses `diskutil` to eject the sd card.  This can be commented out for non-MacOS platforms.  

You will need the SD card menu on the SD card from rosco_m68k/code/software/sdfat_menu.  See documentation [here](https://github.com/rosco-m68k/rosco_m68k/blob/develop/SDCardGuide.md)

