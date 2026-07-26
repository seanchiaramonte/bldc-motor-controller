## 2026-07-08
- Generated initialization code through STM32CubeMX.
- Configured launch.json / tasks.json
- Wrote .gitignore file 
- Uploaded KiCad schematic PDF and .kicad_sch file to the hardware folder

## 2026-07-09
- Flashed LED on STM32 to verify my toolchain
- Created 'make flash' automation in Makefile

## 2026-07-10
- Updated log
- Explained why I decided to use gimbal motor over standard BLDC motor

## 2026-07-13
- Retargeted the _write stub to send data over UART in main.c when printf() is called
- Flashed Coolterm to setup debug window

## 2026-07-14
- Updated decisions folder and explained why I chose to use the DRV8313 breakout board

## 2026-07-15
- Documented the AS5600 breakout board decision and included rationale
- Documented the INA226 breakout board decision and included rationale
- Redesigned the KiCad schematic by removing unnecessary components and adding new net labels
- Updated the hardware folder with the .pdf and .kicad_sch files for the schematic

## 2026-07-17
- Reconfigured STM32MX initialization code to prepare to write code for the motor controller
- Created as5600.h and as5600.c files

## 2026-07-20
- Completed as5600.h and included Doxygen documentation above the AS5600_ReadAngle() function declaration
- Completed as5600.c, defining the as5600's I2C address and the AS5600_ReadAngle() function

## 2026-07-21
- Implemented encoder.h with declarations and documentation for each function
- Implemented encoder.c with mechanical RPM, electrical angle, and sector index calculations
- Cleaned up code for the encoder module

## 2026-07-22
- Implemented the encoder_initialize() function
- Restructured encoder.c so that the functions, Encoder_GetRPM, Encoder_GetElectricalAngle, and Encoder_GetSector, are pure getters
- Restructured encoder.c by adding the Encoder_Update function to allow the getters to be called at any time

## 2026-07-23
- Created pid.h and pid.c files