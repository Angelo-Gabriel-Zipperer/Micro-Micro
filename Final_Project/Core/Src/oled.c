#include "oled.h"
#include <string.h>

// Change this if you end up routing I2C to hi2c2 or hi2c3 in CubeMX
extern I2C_HandleTypeDef hi2c1; 

uint8_t oled_buffer[1024];

void OLED_Init(void) {
    // Standard SSD1306 initialization sequence
    uint8_t init_cmds[] = {
        0xAE,       // Display OFF
        0x20, 0x00, // Set Memory Addressing Mode to Horizontal
        0x21, 0, 127, // Set Column Address (0 to 127)
        0x22, 0, 7,   // Set Page Address (0 to 7)
        0x8D, 0x14, // Enable charge pump
        0xAF        // Display ON
    };
    
    // Send configuration commands (Control byte 0x00)
    for(int i = 0; i < sizeof(init_cmds); i++) {
        HAL_I2C_Mem_Write(&hi2c1, OLED_I2C_ADDR, 0x00, 1, &init_cmds[i], 1, 100);
    }
    
    OLED_Clear();
    OLED_Update();
}

void OLED_Update(void) {
    // Blast the entire 1024-byte buffer to the screen in one go
    // Control byte 0x40 tells the OLED to treat the payload as pixel data
    HAL_I2C_Mem_Write(&hi2c1, OLED_I2C_ADDR, 0x40, 1, oled_buffer, 1024, 100);
}

void OLED_Clear(void) {
    memset(oled_buffer, 0, sizeof(oled_buffer));
}

void OLED_DrawPixel(int x, int y, int color) {
    if (x < 0 || x >= 128 || y < 0 || y >= 64) return;
    
    // The screen is divided into 8 horizontal bands (pages) of 8 pixels high
    int page = y / 8;
    int bit_pos = y % 8;
    
    if (color) {
        oled_buffer[x + (page * 128)] |= (1 << bit_pos);  // Set bit
    } else {
        oled_buffer[x + (page * 128)] &= ~(1 << bit_pos); // Clear bit
    }
}

// The heart of the raycaster: drawing vertical slices of walls
void OLED_DrawVertLine(int x, int y_start, int y_end, int color) {
    if (x < 0 || x >= 128) return;
    
    // Clamp Y values to screen bounds
    if (y_start < 0) y_start = 0;
    if (y_end > 63) y_end = 63;
    if (y_start > y_end) return;

    for (int y = y_start; y <= y_end; y++) {
        OLED_DrawPixel(x, y, color);
    }
}