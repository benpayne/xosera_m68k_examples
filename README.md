# xosera_m68k_examples

These examples assume that the [rosco_m68k](https://github.com/rosco-m68k/rosco_m68k) repo and the [Xosera](https://github.com/XarkLabs/Xosera) repo are next to this repo, but this can be overridden with the following variables:

`ROSCO_M68K_DIR - Location for Rosco dir (this is the same as the examples in Rosco's dir)`
`XOSERA_M68K_DIR - Location of the Xosera dir`

With that complete you should be able to just run make in the example directories and build the code (included dependacies like images).  

You will need to build the `libs` in rosco_m68k and the xosera_m68k_api in Xosera. 

Build insstruction for rosco_m68k can be found [here](https://github.com/rosco-m68k/rosco_m68k/tree/develop/code/software)

Building the Xosera API can be done by running `make m68k` at the top level of the Xosera repo.

## Building this repo

You can build this code by running make at the top level.

Once you've built the code you will need to copy this to a sd card to run on the target system.  There are a number of image assets needed for the programs so best to use the make target `deploy` to help copy everything needed to the SD card.  You will need to set the `DEPLOY_PATH` to achieve this

`DEPLOY_PATH - Location of your SD card on the local filesystem.`

With `DEPLOY_PATH` set, run `make deploy` from the top level dir and all executables and needed files will be copied over to the SD driver.

You will need the SD card menu on the SD card from rosco_m68k/code/software/sdfat_menu.  See documentation [here](https://github.com/rosco-m68k/rosco_m68k/blob/develop/SDCardGuide.md)

