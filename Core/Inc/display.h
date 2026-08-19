#ifndef DISPLAY_H
#define DISPLAY_H

#include "ssd1306.h"
#include <stdio.h>
#include <stdint.h>

/** 
 * @brief Configures the ssd1306 to be written on.
 * 
 * @details 
 * Calls the ssd1306_Init() function which configures the SSD1306 OLED display's settings. After configuration, 
 * the function turns the display on.
 * 
 * @note Function should be called once at boot and I2C1 must be initialized before calling it.
*/
void Display_Initialize(void);

/** 
 * @brief Writes appropriate text onto the display.
 * 
 * @details 
 * Visually resets the display to black and clears the display data stored in RAM. The assembled line then gets written with 
 * the 7x10 font in white. targetRPM and actualRPM are written on the first line and current is written on the second line; 
 * status is written on the third line, displaying either FAULT, STOPPED, or RUNNING.
 * 
 * @note The Display_Initialize function must be called first. This function is called once every 200ms in the display task.
 * 
 * @param actualRPM The actual measured motor speed in RPM.
 * @param targetRPM The commanded motor speed in RPM.
 * @param current The motor supply current in mA.
 * @param motorEN Motor enabled status, 1 indicates enabled.
 * @param systemFault System fault status, 1 indicates fault.
*/
void Display_Update(float actualRPM, float targetRPM, float current, uint16_t motorEN, uint16_t systemFault);

#endif /* DISPLAY_H */