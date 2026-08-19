#include "display.h"

void Display_Initialize(void)
{
    // Chip configuration (orientation, contrast, etc) sent over I2C to the STM32
    // Display is turned on
    ssd1306_Init();
}

void Display_Update(float actualRPM, float targetRPM, float current, uint16_t motorEN, uint16_t systemFault)
{
    ssd1306_Fill(Black); // Visually resets the display to black and clears the display data stored in RAM

    char displayBuffer[20]; // Array size comfortably fits above the worst case scenario of approx. 16 characters

    // Assembles an array while enforcing the buffer limit.
    snprintf(displayBuffer, sizeof(displayBuffer), "RPM: %.1f/%.1f", targetRPM, actualRPM); 
    ssd1306_SetCursor(0, 0); // First line begins at y=0
    ssd1306_WriteString(displayBuffer, Font_7x10, White);

    snprintf(displayBuffer, sizeof(displayBuffer), "Current: %.1fmA", current);
    ssd1306_SetCursor(0, 16); // 6 pixel gap between line 1
    ssd1306_WriteString(displayBuffer, Font_7x10, White);

    if (systemFault == 1) { // Motor faulted
        snprintf(displayBuffer, sizeof(displayBuffer), "Status: FAULT");

    } else if (motorEN == 0) { // Motor stopped
        snprintf(displayBuffer, sizeof(displayBuffer), "Status: STOPPED");

    } else { // Motor running (motorEN == 1)
        snprintf(displayBuffer, sizeof(displayBuffer), "Status: RUNNING");
    }
    ssd1306_SetCursor(0, 32); // 6 pixel gap between line 2
    ssd1306_WriteString(displayBuffer, Font_7x10, White);

    ssd1306_UpdateScreen(); // Sends the updated data to the screen after the content is written
}