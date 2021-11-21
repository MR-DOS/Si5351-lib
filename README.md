# Si5351-lib
----
## Description
Si5351-A/B/C-B lib for STM32 Cortex M processors. Later, it may be modified for other architectures as well. If you wih to port it to another architecture, you just need to replace 2 functions which deal with I2C and one member of one struct which specifies which I2C to use.

----
## Overview
This library can (contrary to most other libraries) completely utilize the features of Si5351-A/B/C-B (second generation of Si5351-A/B/C). Currently, it can set all of the properties of input stage, PLLs, Multisynths, CLKs and output stage. It can also set several undocumented features which (at least I think so) were not yet implemented anywhere else - for example turn off "fanout" of several internal signals or modify the capacitive load of the internal VCO.

The approach of this library is not to completely abstract from the hardware of the PLL - you have to figure out the frequency plan yourself. This library handles all of the communication and formatting of the parameters and cuts off user from having to care about some details like switching between integer and fractional modes (this happens automatically inside the library).

Reset of PLLs is not yet completely automated, because it is very badly documented, so it is still a responsibility of the user.

----
## Features
### Input Stage
All of the important features are implemented - CLKIN divider, VCXO, crystal oscillator load setting and PLL capacitive load (yep, maybe it should be moved to other section).
The undocumented 4 pF crystal load is included.

TODO: Automatically detect if it is possible to turn off "fanout" of internal clocks.

### PLL
The PLL can be set to any frequency - the limits were empirically found to be about 170 - 1200 MHz where the VCO can oscillate.
Useable range of frequencies is however (on my tested devices) narrower - 170 to 820 MHz (yes, one is out of spec, the other is just abusement of the device, it should be 600-900).
The library takes control of everything, all you need is to set the multiplier values and clock source and use the configuration functions. It is also possible to override the default VCO capacitive load, though not recommended.

NOTE: Resetting both PLLs at the same time has been implemented. Note however that even by resetting them at the same time, they will never be synchronised! If you want synchronous clocks with defined phase offset, they have to be taken from one PLL!

### Spread Spectrum
Spread spectrum supports both down- and center-spread. It has been empirically found that it can be used up to about 5% spread (datasheet allows max +-2.5%). At around 8%, glitches on output start to appear.

TODO: Add possibility to set SS_NCLK by user. This however needs to be first tested using a spectral analyzer, because SiLabs are trying not to help at all.

### MultiSynth
The MultiSynth can be set to any integer or fractional divider and use any of the PLLs. The library automatically sets the mode depending on the settings you give it.
The minimal divider is 4 (this allows for max output clock of 205 MHz on my devices), maximal is 1800 (111 kHz when PLL=200 MHz). Minimal divider for fractional mode is 8. Multisynths 6 and 7 which are integer-only are implemented as well.

### Output stage
Output R divider, clock selection, phase offset, inversion, OEB pin masking, current drive, disable state, enabling and power down are supported.

### Interrupts
Interrupts can be masked. Even the undocumented LOS\_XTAL\_MASK is implemented.

### Status bits
Reading status bits is supported. Reading and clearing "sticky" status bits is implemented as well. The half-documented LOS_XTAL is implemented.

----
## The source files
Look in the folders "inc" and "src" for source files. In folder "documentation", you may find all relevant info about the Si5351.

----
## Porting
Information for anyone wishing to make a port are included in the source files. In short, all you need to modify, is the two communication functions: Si5351\_WriteRegister and Si5351\_ReadRegister and the I2Cx member of the struct Si5351\_ConfigTypeDef.

----
## Coming potentially soon
There should be some automatic way of reseting the PLL only when it is needed to relieve the user of this responsibility.
SS_NCLK parameter could be a bit investigated and maybe abused for something useful.
There are still several strange unexplained bits in register map which may be of interest.
Maybe I will make it platform independent, so it could be used at least with Arduino as well.


----
## Example
Simple example how to get 50 kHz output with 25 MHz crystal:
```C
Si5351_ConfigTypeDef Si5351_ConfigStruct;

Si5351_StructInit(&Si5351_ConfigStruct);	//initialize the structure with default "safe" values
Si5351_ConfigStruct.OSC.OSC_XTAL_Load = XTAL_Load_8_pF;	//use 8 pF load for crystal

Si5351_ConfigStruct.PLL[0].PLL_Clock_Source = PLL_Clock_Source_XTAL;	//select xrystal as clock input for the PLL
Si5351_ConfigStruct.PLL[0].PLL_Multiplier_Integer = 32;	//multiply the clock frequency by 32, this gets us 800 MHz clock

Si5351_ConfigStruct.MS[0].MS_Clock_Source = MS_Clock_Source_PLLA;	//select PLLA as clock source for MultiSynth 0
Si5351_ConfigStruct.MS[0].MS_Divider_Integer = 250;	//divide the 800 MHz by 250 this gets us 3.2 MHz

Si5351_ConfigStruct.CLK[0].CLK_R_Div = CLK_R_Div64;	//divide the MultiSynth output by 64, this gets us 50 kHz
Si5351_ConfigStruct.CLK[0].CLK_Enable = ON;	//turn on the output

Si5351_Init(&Si5351_ConfigStruct);	//apply the changes
```
This is all you need to get a clock out of the Si5351. There are many options you may explore, but this is the very basic mandatory setting.

----
## Reference
Please, be aware that the documentation of SiLabs is full of errors and nonsense. My library has most of them 
fixed, some of my own fixes were even reflected by SiLabs in AN-619 v0.7. I am hosting the documents here, because SiLabs hides the old versions of the documents.
PLEASE BE AWARE OF THE FACT THAT THE LIBRARY HAS BEEN WRITTEN IN 2018-2020. Therefore, it should be updated to 
match datasheet 1.2 and 1.3 and AN-619 0.8. Also, older versions should be checked fro explaining undocumented registers.

Datasheets:

[Datasheet for the Si5351 v1.3 NOT USED DURING LIBRARY CREATION](/documentation/datasheet_1.3.pdf)

[Datasheet for the Si5351 v1.2 NOT USED DURING LIBRARY CREATION](/documentation/datasheet_1.2.pdf)

[Datasheet for the Si5351 v1.1](/documentation/datasheet_1.1.pdf)

[Datasheet for the Si5351 v1.0](/documentation/datasheet_1.0.pdf)

[Datasheet for the Si5351 v0.75 NOT USED DURING LIBRARY CREATION](/documentation/datasheet_0.75.pdf)

[Datasheet for the Si5351 v0.95 (it is older than 0.75) NOT USED DURING LIBRARY CREATION](/documentation/datasheet_0.95.pdf)

[Datasheet for the Si5351 v0.9 (it is older than 0.75) NOT USED DURING LIBRARY CREATION](/documentation/datasheet_0.9.pdf)

[Datasheet for the Si5351 v0.1 NOT USED DURING LIBRARY CREATION](/documentation/datasheet_0.1.pdf)

Application note explaining setup of the chip:

[AN-619: Manually generating a Si5351 Register Map v0.8 NOT USED DURING LIBRARY CREATION](/documentation/AN619_0.8.pdf)

[AN-619: Manually generating a Si5351 Register Map v0.7 with my own corrections of bugs](/documentation/AN619_0.7_corrected.pdf)

[AN-619: Manually generating a Si5351 Register Map v0.7](/documentation/AN619_0.7.pdf)

[AN-619: Manually generating a Si5351 Register Map v0.6 (This is the only place where there is some mention of the VCO capacitive load)](/documentation/AN619_0.6.pdf)

[CURRENTLY NOT KNOWN ~~AN-619: Manually generating a Si5351 Register Map v0.5 NOT USED DURING LIBRARY CREATION~~]()

[CURRENTLY NOT KNOWN ~~AN-619: Manually generating a Si5351 Register Map v0.4 NOT USED DURING LIBRARY CREATION~~]()

[AN-619: Manually generating a Si5351 Register Map v0.3 NOT USED DURING LIBRARY CREATION](/documentation/AN619_0.3.pdf)

[AN-619: Manually generating a Si5351 Register Map v0.2 NOT USED DURING LIBRARY CREATION](/documentation/AN619_0.2.pdf)

[CURRENTLY NOT KNOWN ~~AN-619: Manually generating a Si5351 Register Map v0.1 NOT USED DURING LIBRARY CREATION~~]()

Application note explaining setup of the chip for the 4-channel version of Si5351:

[AN-1234: Manually Generating an Si5351 Register Map for 16QFN Devices v0.1](/documentation/AN1234_0.1.pdf)

Auxiliary application notes:

[AN-551: Crystal selection guide for Si5350/51 devices v0.3](/documentation/AN551.pdf)

[CURRENTLY NOT KNOWN ~~AN-551: Crystal selection guide for Si5350/51 devices v0.2 NOT USED DURING LIBRARY CREATION~~]()

[AN-551: Crystal selection guide for Si5350/51 devices v0.1 NOT USED DURING LIBRARY CREATION](/documentation/AN551_0.1.pdf)

[AN-554: Si5350/51 PCB Layout Guide v0.3](/documentation/AN554.pdf)

[AN-554: Si5350/51 PCB Layout Guide v0.2 NOT USED DURING LIBRARY CREATION](/documentation/AN554_0.2.pdf)

[AN-554: Si5350/51 PCB Layout Guide v0.1 NOT USED DURING LIBRARY CREATION](/documentation/AN554_0.1.pdf)

FAQ (list of versions not available)

[Si5351 FAQ v0.5 NOT USED DURING LIBRARY CREATION](/documentation/Si5350-Si5351FAQ(v0.5).pdf)

[Si5351 FAQ v0.2](/documentation/Si5350-Si5351FAQ(v0.2).pdf)

Here is a list of errors I found in datasheet 1.0 and AN-619 0.6:

[DELETED after SiLabs being bought by Skyworks ~~First contact with someone from SiLabs about all of those nasty bugs~~](https://www.silabs.com/community/timing/forum.topic.html/si5351_hw_bugs_-out-KeNr)

Effort to find some info about the VCO capacitive load and the LOS_XTAL and its corresponding status bit, sticky bit and int mask:

[DELETED after SiLabs being bought by Skyworks ~~Second contact with SiLabs about the bugs, a year later~~](https://www.silabs.com/community/timing/forum.topic.html/si5351_register_183-r4JV)

And most importantly - some info from radioamateurs who were trying out how the damned thing works:

[G0UPL (Hans Summers) and some of his and others wisdom](https://groups.io/g/BITX20/topic/si5351a_facts_and_myths/5430607?p=Created,,,20,1,0,0&jump=1)
