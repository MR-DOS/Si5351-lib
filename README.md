# Si5351-lib
## Description
Si5351-A/B/C-B lib for STM32 Cortex M processors. Later, it may be modified for other architectures as well.

## Overview
This library can (contrary to most other libraries) completely utilize the features of Si5351-A/B/C-B (second generation of Si5351-A/B/C). Currently, it can set all of the properties of input stage, PLLs, Multisynths and output stage.

The approach of this library is not to completely abstract from the hardware of the PLL - it does not compute any of the divider / multiplier values. This has to be done by the user or by another abstraction library sitting on top of this library. This library handles all of the communication and formatting of the parameters.

## Features
### Input Stage
All of the important features are implemented - CLKIN divider, VCXO, crystal oscillator load setting.
The undocumented 4 pF crystal load is included.
TODO: Add functions to allow setting "fanout enable" for XO and CLKIN. Now the fanout is always on.

### PLL
The PLL can be set to any frequency - the limits were empirically found to be 185 - 1200 MHz where the VCO can oscillate.
Useable range of frequencies is however (on my tested devices) narrower - 200 to 820 MHz (yes, one is out of spec, the other is just abusement of the device, it should be 600-900).
The library takes control of everything, all you need is to set the multiplier values and clock source and use the configuration functions.

### Spread Spectrum
Spread spectrum supports both down- and center-spread. It has been empirically found that it can be used up to about 5% spread (datasheet allows max +-2.5%).

### MultiSynth
The MultiSynth can be set to any integer or fractional divider and use any of the PLLs. The library automatically sets the mode depending on the settings you give it.
The minimal divider is 4 (this allows for max output clock of 205 MHz on my devices), maximal is 1800 (111 kHz when PLL=200 MHz). Minimal divider for fractional mode is 8.

### Output stage
Output R divider, clock selection, phase offset, inversion, OEB pin masking, current drive, disable state, enabling and power down are supported.

## The source files
Look in the folders "inc" and "src" for source files.
Look in the folder "example" for example project. The project was created in System workbench for STM32 (SW4STM32).

## Porting
Information for anyone wishing to make a port are included in the source files. In short, all you need to modify, is the two communication functions: Si5351_WriteRegister and Si5351_ReadRegister.

## Coming potentially soon
All of the code generating values for Multisynth (both feedback and output MS) P1, P2 and P3 should be checked - there may be some error. There should be some automatic way of reseting the PLL only when it is needed to relieve the user.

## Reference
[Datasheet for the Si5351](https://cdn-shop.adafruit.com/datasheets/Si5351.pdf)

[AN-551: Crystal selection guide for Si5350/51 devices](https://www.silabs.com/documents/public/application-notes/AN551.pdf)

[AN-554: Si5350/51 PCB Layout Guide](https://www.silabs.com/documents/public/application-notes/AN554.pdf)

[AN-619: Manually generating a Si5351 Register Map](https://www.silabs.com/documents/public/application-notes/AN619.pdf)

And most importantly - some info from radioamateurs who were trying out how the damned thing works:

[G0UPL (Hans Summers) and some of his and others wisdom](https://groups.io/g/BITX20/topic/si5351a_facts_and_myths/5430607?p=Created,,,20,1,0,0&jump=1)
