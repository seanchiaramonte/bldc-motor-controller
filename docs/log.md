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