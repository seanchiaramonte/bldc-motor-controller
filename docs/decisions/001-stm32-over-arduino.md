# 001: Use STM32 NUCLEO-F446RE over Arduino

## Context: Need to drive three PWM signals to the motor, run UART, have I2C working on three separate peripherals, run multiple different tasks at once with reliable programming/debugging (especially for PID). Arduino boards in the same price range have worse performance, less peripherals, and less debugging capabilities.

## Decision: Use STM32 NUCLEO-F446RE over an Arduino Uno or similar boards.

## Consequences:

**Positives**
    - Faster debugging with ST-LINK
    - Higher performance: 8-bit AVR vs. 32-bit ARM with FPU, 16 MHz vs. 180 MHz clock speed, 32KB Flash vs. 512 KB SRAM & 2 KB SRAM vs. 128 KB SRAM
    - Includes three I2C buses, so each component gets its own I2C bus

**Negatives**
    - Harder learning curve