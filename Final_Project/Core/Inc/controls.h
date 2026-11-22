#ifndef CONTROLS_H
#define CONTROLS_H

#include "main.h"

// Map your 4 kit buttons to game actions
typedef enum {
    BTN_SHOOT = 0,   // PA11
    BTN_INTERACT,    // PA12 (open doors / switches)
    BTN_WEAPON,      // PB4  (switch weapon)
    BTN_PAUSE,       // PB2  (pause / save game)
    BTN_COUNT
} GameButton;

void Controls_Init(void);
float Controls_GetMove(void);                  // Returns -1.0f (back) to +1.0f (forward)
float Controls_GetTurn(void);                  // Returns -1.0f (left) to +1.0f (right)
uint8_t Controls_ReadButton(GameButton btn);   // Returns 1 once per button press
void Controls_HandleEXTI(uint16_t GPIO_Pin);   // Call inside HAL_GPIO_EXTI_Callback

#endif