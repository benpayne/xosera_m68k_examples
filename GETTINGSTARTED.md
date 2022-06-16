# Xosera Setup Guide
By Ben Payne (CryptoBen)

## Building the Board
The PCB for the board can be found in the hardware project repo on github https://github.com/rosco-m68k/hardware-projects.
The KiCAD files are in this repo. You can use this to have a PCB (or a few made) from a variety of sources.  Since you 
typically get 3-5 as a min run size.  Many of use may have an extra board or two around.  So reach out to the 
xosera-users channel on discord and you might find someone that has one to available, to sell for a small fee.

Once you have the PCB procured you'll need to look at getting the parts.  A BOM is included 
https://raw.githubusercontent.com/rosco-m68k/hardware-projects/master/xosera/design/kicad/xosera.csv
The value for R1-8 is missing in the BOM, it's 470 Ohm.  In addition you'll want to think about sockets for the chips.  
It's always good to be able to replace them if sometihng goes wrong.  And for the PLD chips it's always possible that a bug or a improvement is made down the road.  As for the 
upduino it's a good idea to socket this as well.  This can be done with two 24 pin single-row female headers (or some 
longer ones trimmed to size).  If you trim a longer one make sure to cut at the 25th pin, so you don't damage the 24th 
pin when you cut.  

Other things to note on the BOM, the PLD chips are out of production and typically not available on mainstream 
electronics distributors like Digikey or Mouser.  So best to look at eBay and AliExpress for these.  

The PMOD interface for VGA is available https://digilent.com/shop/pmod-vga-video-graphics-array/

Checkout this guide to the board and jumper settings.  But key is to note forget to modify the Upduino OSC clock.
https://github.com/rosco-m68k/rosco_m68k/blob/develop/docs/Xosera%20r1%20Information.pdf

## Programming PLD Chips
you will need minipro tools to help you program the PLD chips.  You will also need the hardware projects repo for the 
PLD code.  There is pre-syntizied bin files of the code for the chips already here.  So no need for the tools to 
synthesise this.  If you curious how to synthesise the code check HEREURL.  

You will also need the programmer itself.  This device will work HEREURL.  This is the same programmer than can be
used to program ROM chips for the rosco board.  So if you want to build and upgrade your ROMs this is your best bet.  


### Decoder
`cd hardware-project/xosera/pld/decoder`

Then run:

`./burn.sh`

Also note that I get errors about my firmware version being older than what minipro expects.  This doesn't seem to 
cause any issue and can be ignored.  I did eventually download the offical windows software and update the firmware to 
the latest, but minipro still complains because now my firmware is newer than the version it "expects".  

### Glue
Follow a similar process for the decoder:

`cd hardware-project/xosera/pld/glue`

Then run:

`./burn.sh`

I did have one issues in my build.  My chips were lattice GAL22v10's and the would not work with the defaults that 
are in the burn.sh file.  So I ran the following command if my case to get them to properly program.  

`GAL_PART=GAL22V10B ./burn.sh`

## Building the FPGA Code (optional)
This step is not needed since the built version of the code is included in the repo.  However if you want to modify 
the firmware in any way then you would need this step.  This is already documneted in the Building.md file in the Xosera 
repo

## Programming the FPGA
You will need to have the oss_cad_suite tools installed to program the FPGA.  You will then need to have the Xosera git 
repo checked out.  from the top level of this repo you can run make commands to build and flash the system.  See 
Building the FPGA code for building 

## Testing and Hookup
When connecting this to your rosco mainboard, pay close attention to getting the correct orientation.  Also make sure that 
before you solder on the 68 pin connector that you ensure it will work with what your plugging it into.  Typically iy will
be on the front of the PCB.  That is true when plugging directly into the mainboard or when connecting to the bus board.  

A good test for the programming of the FPGA is to power it on over the Upduinos USB port when it connected to the Xosera 
board.  Connect a monitor to the PMOD VGA connector.  You should get a grey screen.  If not there is something wrong with
the program, also ensure you shorted the OSC clock pin.  

Once I got the board in the proper orientation and shorted the OSC pin, I was able to get this booting without issues.

You will want to have a version 1.3 or later firmware on your board to make sure that the boot loader support the Xosera 

## Running Rosco Test Programs
