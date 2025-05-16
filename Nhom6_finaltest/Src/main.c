#include "stm32f401re.h"
#include "stm32f401re_gpio.h"
#include "stm32f401re_rcc.h"

// Define pin numbers for LEDs
#define LED_GREEN GPIO_Pin_5       // Power indicator LED (PA5)
#define LED_RED   GPIO_Pin_1       // RGB RED LED (PA1)
#define LED_2     GPIO_Pin_11      // Secondary GREEN LED (PA11)

// Define GPIO ports and pins for buttons
#define BUTTON_B2_GPIO_PORT GPIOB
#define BUTTON_B2_PIN       GPIO_Pin_3
#define BUTTON_B4_GPIO_PORT GPIOB
#define BUTTON_B4_PIN       GPIO_Pin_0

// Define GPIO port and pin for buzzer
#define BUZZER_GPIO_PORT        GPIOC
#define BUZZER_GPIO_PIN         GPIO_Pin_9
#define BUZZERControl_SetClock  RCC_AHB1Periph_GPIOC

// Function prototypes
void Delay_ms(uint32_t ms);
void BlinkLED(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, int times, int delay);
void BuzzerBeepWithLED(int times, int delay);
void GPIO_Config(void);

int main(void) {
    GPIO_Config();  // Initialize GPIOs
    BlinkLED(GPIOA, LED_GREEN, 4, 0.25);  // Blink power indicator LED 4 times

    int press_count_B2 = 0;
    uint8_t previous_state_B2 = 0;
    uint16_t hold_counter_B4 = 0;
    const uint16_t hold_threshold = 125; // ~500ms if loop delay is ~4ms

    while (1) {
        // Handle B2 button press (count to 4)
        uint8_t current_state_B2 = GPIO_ReadInputDataBit(BUTTON_B2_GPIO_PORT, BUTTON_B2_PIN);
        if (current_state_B2 == Bit_SET && previous_state_B2 == Bit_RESET) {
            Delay_ms(50); // Debounce delay
            if (GPIO_ReadInputDataBit(BUTTON_B2_GPIO_PORT, BUTTON_B2_PIN) == Bit_SET) {
                press_count_B2++;
            }
        }
        previous_state_B2 = current_state_B2;

        if (press_count_B2 == 5) {
            BuzzerBeepWithLED(4, 250);  // Buzz and blink LED 4 times
            press_count_B2 = 1;
        }

        // Handle B4 button hold
        if (GPIO_ReadInputDataBit(BUTTON_B4_GPIO_PORT, BUTTON_B4_PIN) == Bit_RESET) {
            hold_counter_B4++;
            if (hold_counter_B4 >= hold_threshold) {
                GPIO_SetBits(GPIOA, LED_2);  // Turn on LED_2
                GPIO_SetBits(BUZZER_GPIO_PORT, BUZZER_GPIO_PIN);  // Turn on buzzer
            }
        } else {
            hold_counter_B4 = 0;
            GPIO_ResetBits(GPIOA, LED_2);  // Turn off LED_2
            GPIO_ResetBits(BUZZER_GPIO_PORT, BUZZER_GPIO_PIN);  // Turn off buzzer
        }

        Delay_ms(0.125);  // Small delay to measure hold time
    }
}

// Blink an LED for a given number of times and delay
void BlinkLED(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, int times, int delay) {
    for (int i = 0; i < times; i++) {
        GPIO_SetBits(GPIOx, GPIO_Pin);
        Delay_ms(delay);
        GPIO_ResetBits(GPIOx, GPIO_Pin);
        Delay_ms(delay);
    }
}

// Beep the buzzer and flash LED together
void BuzzerBeepWithLED(int times, int delay) {
    for (int i = 0; i < times; i++) {
        GPIO_SetBits(GPIOA, LED_RED);  // Turn on RED LED
        GPIO_SetBits(BUZZER_GPIO_PORT, BUZZER_GPIO_PIN);  // Turn on buzzer
        Delay_ms(delay);
        GPIO_ResetBits(GPIOA, LED_RED);  // Turn off RED LED
        GPIO_ResetBits(BUZZER_GPIO_PORT, BUZZER_GPIO_PIN);  // Turn off buzzer
        Delay_ms(delay);
    }
}

// Rough delay function (approximate, based on NOP instructions)
void Delay_ms(uint32_t ms) {
    for (uint32_t i = 0; i < ms * 4000; i++) {
        __NOP();  // No operation (just waste time)
    }
}

// Configure GPIO pins for LEDs, buttons, and buzzer
void GPIO_Config(void) {
    // Enable clocks for GPIOA, GPIOB, GPIOC
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

    GPIO_InitTypeDef GPIO_InitStruct;

    // Initialize output pins for LEDs (PA5, PA1, PA11)
    GPIO_InitStruct.GPIO_Pin = LED_GREEN | LED_RED | LED_2;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // Initialize output pin for buzzer (PC9)
    GPIO_InitStruct.GPIO_Pin = BUZZER_GPIO_PIN;
    GPIO_Init(BUZZER_GPIO_PORT, &GPIO_InitStruct);

    // Initialize input pin for button B2 (PB3) with pull-down resistor
    GPIO_InitStruct.GPIO_Pin = BUTTON_B2_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_Init(BUTTON_B2_GPIO_PORT, &GPIO_InitStruct);

    // Initialize input pin for button B4 (PB0) with pull-down resistor
    GPIO_InitStruct.GPIO_Pin = BUTTON_B4_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_Init(BUTTON_B4_GPIO_PORT, &GPIO_InitStruct);
}
