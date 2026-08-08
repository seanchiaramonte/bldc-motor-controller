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

## 2026-07-26
- Updated log.md

## 2026-07-28
- Implemented the PID_Initialize() and PID_Reset() functions 
- Added Doxygen documentation for both functions

## 2026-07-29
- Implemented the PID_Update function with its Doxygen documentation

## 2026-08-02
- Fixed PID_Initialize() to zero integral and previousRPM values
- Updated the PID_Initialize() Doxygen comment in pid.h

## 2026-08-03
- Updated log.md

## 2026-08-04
- Wrote temporary code block in main.c to test and debug as5600.c and encoder.c
- Connected the GND, VCC, SCL, and SDA pins to the STM32 with jumper wires
- Confirmed as5600 and encoder modules work by sending angle, electrical angle, sector, and RPM values to CoolTerm
- Fixed make error by adding as5600.c and encoder.c files to Makefile

## 2026-08-05
- Documented the decision to use a single I2C bus via decision 006 in the decisions folder
- Documented the decision to use trapezoidal commutation via decision 007 in the decisions folder

## 2026-08-06
- Documented decision to use the HM-10 BLE module via decision 008 in the decisions folder
- Documented decision to use the SSD1306 OLED display via decision 009 in the decisions folder
- Created motor.h and motor.c files
- Implemented the Motor_Initialize() function in motor.c with Doxygen doc in motor.h
- Implemented the Motor_Enable() and Motor_Disable() functions in motor.c with Doxygen docs in motor.h

## 2026-08-07
- Implemented Motor_ApplyCommutation() function in motor.c with Doxygen doc in motor.h
- Implemented Motor_CheckFault() function in motor.c with Doxygen doc in motor.h
- Updated log.md