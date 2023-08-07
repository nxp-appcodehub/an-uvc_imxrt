Overview
========
This demo application shows how to build a USB video device based on MCUXpresso SDK.

Toolchain supported
===================
- IAR embedded Workbench 8.20

Hardware requirements
=====================
- Two micro USB cables
- EVK-MIMXRT1050 board
- MT9M114 camera module
- Personal Computer

Board settings
==============
Solder these zero Ohm resistors if they are not soldered: R217, R218, R220, R221,
R222, R223, R224, R225, R226, R227, R228, R229

Prepare the Demo
================
1. Connect the MT9M114 camera module to J35 on the EVK-MIMXRT1050 board.
2. Connect a micro USB cable between the host PC and the OpenSDA USB port J28 on the EVK-MIMXRT1050 board. 
3. Connect another micro USB cable between the host PC and the micro USB port J9 on the EVK-MIMXRT1050 board. 
4. Open a serial terminal with the following settings:
    - 115200 baud rate
    - 8 data bits
    - No parity
    - One stop bit
    - No flow control
5. Build a configuration and download the program to the target board.

Running the demo
================
If you debug the demo with the "sdram_txt" configuration, launch the debugger in the
IDE to begin running the demo. If you have downloaded the binary image generated
by the "flexspi_nor" configuration, press the reset button SW4 on the board to 
begin running the demo.
The log below shows the output of the demo in the terminal window:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
This is a USB video demo application.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
A USB device named "CSI Camera Device" start to be enumerated. After the 
enumeration is finished, open a webcam program on your PC, like AMCap. Select
and open the demo video device. Then you can see the video screen on your PC.

