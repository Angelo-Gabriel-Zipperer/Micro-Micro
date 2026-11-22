#include "controls.h"

// Matches the ADC handle CubeMX generates (change if you use hadc2)
extern ADC_HandleTypeDef hadc1;

#define ADC_CENTER    2048
#define ADC_DEADZONE  250   // Ignores slight thumbstick wobble near center
#define ADC_MAX_SPAN  1798  // 2048 - 250

// DMA writes X and Y joystick values here automatically in the background
static volatile uint16_t joy_raw[2] = {ADC_CENTER, ADC_CENTER};
static volatile uint8_t btn_pressed[BTN_COUNT] = {0};

static float Normalize_Axis(uint16_t raw) {
    int32_t offset = (int32_t)raw - ADC_CENTER;

    if (offset > -ADC_DEADZONE && offset < ADC_DEADZONE) {
        return 0.0f;
    }

    if (offset > 0) {
        offset -= ADC_DEADZONE;
    } else {
        offset += ADC_DEADZONE;
    }

    float normalized = (float)offset / (float)ADC_MAX_SPAN;
    if (normalized > 1.0f) normalized = 1.0f;
    if (normalized < -1.0f) normalized = -1.0f;

    return normalized;
}

void Controls_Init(void) {
    // Calibrate STM32G4 ADC for accurate center readings, then start DMA
    HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)joy_raw, 2);
}

float Controls_GetTurn(void) {
    return Normalize_Axis(joy_raw[0]); // Channel 1: Joystick X-axis
}

float Controls_GetMove(void) {
    // Invert sign if pushing stick forward gives a lower ADC voltage on your kit
    return Normalize_Axis(joy_raw[1]); // Channel 2: Joystick Y-axis
}

uint8_t Controls_ReadButton(GameButton btn) {
    if (btn >= BTN_COUNT) return 0;

    if (btn_pressed[btn]) {
        btn_pressed[btn] = 0; // Clear flag so one press triggers one shot
        return 1;
    }
    return 0;
}

void Controls_HandleEXTI(uint16_t GPIO_Pin) {
    static uint32_t last_tick[BTN_COUNT] = {0};
    uint32_t now = HAL_GetTick();
    GameButton btn = BTN_COUNT;

    if (GPIO_Pin == GPIO_PIN_11)      btn = BTN_SHOOT;
    else if (GPIO_Pin == GPIO_PIN_12) btn = BTN_INTERACT;
    else if (GPIO_Pin == GPIO_PIN_4)  btn = BTN_WEAPON;
    else if (GPIO_Pin == GPIO_PIN_2)  btn = BTN_PAUSE;

    if (btn < BTN_COUNT && (now - last_tick[btn] >= 200)) {
        last_tick[btn] = now;
        btn_pressed[btn] = 1;
    }
}