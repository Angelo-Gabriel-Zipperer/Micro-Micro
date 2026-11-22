#ifndef OLED_H
#define OLED_H

#include "main.h" // Pulls in your specific STM32 HAL headers

// Standard I2C address for 0.96" SSD1306 OLEDs (0x3C shifted left by 1)
#define OLED_I2C_ADDR 0x78 

// The 128x64 screen represented in memory (128 columns * 8 pages = 1024 bytes)
extern uint8_t oled_buffer[1024];

void OLED_Init(void);
void OLED_Clear(void);
void OLED_Update(void);
void OLED_DrawPixel(int x, int y, int color);
void OLED_DrawVertLine(int x, int y_start, int y_end, int color);

#endif