# Lathe Electronic Leadscrew Controller with MPG and Lever Controlled feed

## Based on code written and tested by Clough42. 
Without him, this mod would not exist. 

This is the firmware for an experimental lathe electronic leadscrew controller.  In addition to replacing the change
gears or gearbox on a metalworking lathe with a stepper motor controlled electronically based on an encoder on the
lathe spindle, it adds features like controlling feed with a left/right lever and adjusting exact position using a 
generic Manual Pulse Generator (Handwheel encoder).  The electronic controller allows setting different virtual gear 
ratios for feeds and threading. This version allows you to completely throw away all leadscrews, feed rods, halfnuts, 
etc. from your lathe and replace all that with a single stepper and a single leadscrew / ballscrew with the nut 
permanently locked on the screw. Be aware that it does not simulate a virtual halfnut so in threading mode you still
need to reverse the spindle to go back.

## Project Status
It's alive!!!

**NOTE: This is still a work in progress and is subject to change.  Moreover, it has only been tested on my system
and most likely will break if some variables are changed. Prepare to spend a lot of time debugging 
if something goes wrong.**

## The hardware looks like this:
* TI F280049C Piccolo microcontroller on a LaunchXL prototyping board
* SPI "LED&Key" import display and keypad for the user interface
* 1024-line encoder on the spindle via a 2:1 timing belt
* Standard stepper motor driving a ballscrew, nut is fixed to the apron
* Two-position lever or two non-locking buttons to feed forward or reverse
* Generic 100ppr (clicks per revolution) MPG handwheel
* Most useful for those whose lathe either has no leadscrew originally or who plans to remove it completely

## Following operations are supported:
* Multiple feeds in hundredths of a millimeter per revolution
* Multiple feeds in thousandths of an inch per revolution
* Multiple metric thread pitches (in millimeters)
* Multiple imperial thread pitches (in threads-per-inch)
* In feed mode, the lever or physical buttons control the forward/backward movement or stop the leadscrew
* In feed mode at any time the position can be adjusted by turning the MPG wheel. This works regardless of 
the feed being forward, stopped or backward. Lever control and MPG are disabled in threading mode.

## Hardware Configuration
> NOTE: the silkscreen on the LaunchXL-F280049C board has the labels for J6 and J8
> swapped by the factory.  J6 is the header closest to the edge of the board.

If you want to get this running on your own LaunchXL-F280049C board, here are the settings you will need:

### Switches on the LaunchXL Board
The LaunchXL-F280049C has a number of switches on the board to configure the GPIO and peripheral signal routing.
This provides lots of flexibility for different uses of the board, but it means that you will have to change
some of the switches from their defaults in order to use the GPIO pins and eQEP periperals.

These are the required switch settings
* `S3.1 = 0` Connects eQEP1 to J12
* `S3.2 = 0` Connects eQEP2 to J13
* `S4 = 1` Connects eQEP1 to J12
* `S9 = 1` (default) -- Connects GPIO32/33 to BoosterPack site 2 pins

S6 and S8 can be in any position.  0 is the default.

### Physical Feed Switch
The firmware assumes the following pins are connected to two normally open buttons with common ground or 
a two-position lever (ON1-OFF-ON2).
* `GPIO12` (J6 pin 51) - Left (forward) switch
* `GPIO34` (J6 pin 52) - Right (backward) switch

### Encoder
A quadrature encoder for the spindle is connected to the eQEP connector, J12. If your encoder has a "Z" output, 
connect it to "I" on the eQep. You can even leave it disconnected - it is not used in the code anyway.
The encoder is getting 5V power from the board and communicates via 5V pull-up pins.

###MPG
A generic import 5V 100PPR MPG handwheel is connected to eQep2 connector, J13. "I" pin is not used.
The encoder is getting 5V power from the board and communicates via 5V pull-up pins.

### Stepper Driver
A Step-direction stepper motor driver should be connected to the following GPIO pins:
* `GPIO0` (J8 pin 80) - Step
* `GPIO1` (J8 pin 79) - Direction
* `GPIO6` (J8 pin 78) - Enable
* `GPIO7` (J8 pin 77) - Alarm input (not used so do not connect)

### Control Panel
The LED&KEY control panel board **must be connected through a bidirectional level converter**, since
it is a 5V device, and the TI microcontroller is a 3.3V device.  A standard BSS138-based converter works
well.
* `GPIO24` (J6 pin 55) - DIO
* `GPIO32` (J8 pin 71) - CLK
* `GPIO33` (J6 pin 53) - STB (chip select)

