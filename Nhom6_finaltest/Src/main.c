#include "stm32f401re.h"
#include "stm32f401re_gpio.h"
#include "stm32f401re_rcc.h"

#define LED_GREEN GPIO_Pin_5       // LED cấp nguồn (PA5)
#define LED_RED   GPIO_Pin_1       // LED RGB RED (PA1)
#define LED_2     GPIO_Pin_11      // LED_2 GREEN (PA11)

#define BUTTON_B2_GPIO_PORT GPIOB
#define BUTTON_B2_PIN       GPIO_Pin_3
#define BUTTON_B4_GPIO_PORT GPIOB
#define BUTTON_B4_PIN       GPIO_Pin_0

#define BUZZER_GPIO_PORT        GPIOC
#define BUZZER_GPIO_PIN         GPIO_Pin_9
#define BUZZERControl_SetClock    RCC_AHB1Periph_GPIOC

void Delay_ms(uint32_t ms);
void BlinkLED(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, int times, int delay);
void BuzzerBeepWithLED(int times, int delay);
void GPIO_Config(void);

int main(void) {
    GPIO_Config();
    BlinkLED(GPIOA, LED_GREEN, 4, 0.25);  // LED báo nguồn

    int press_count_B2 = 0;
    uint8_t previous_state_B2 = 0;
    uint16_t hold_counter_B4 = 0;
    const uint16_t hold_threshold = 125; // ~500ms nếu mỗi vòng mất ~4ms

    while (1) {
        // xử lý nút B2 (nhấn 4 lần)
        uint8_t current_state_B2 = GPIO_ReadInputDataBit(BUTTON_B2_GPIO_PORT, BUTTON_B2_PIN);
        if (current_state_B2 == Bit_SET && previous_state_B2 == Bit_RESET) {
            Delay_ms(50); // chống dội
            if (GPIO_ReadInputDataBit(BUTTON_B2_GPIO_PORT, BUTTON_B2_PIN) == Bit_SET) {
                press_count_B2++;
            }
        }
        previous_state_B2 = current_state_B2;

        if (press_count_B2 == 5) {
            BuzzerBeepWithLED(4, 250);
            press_count_B2 = 1;
        }

        // xử lý giữ nút B4
        if (GPIO_ReadInputDataBit(BUTTON_B4_GPIO_PORT, BUTTON_B4_PIN) == Bit_RESET) {
            hold_counter_B4++;
            if (hold_counter_B4 >= hold_threshold) {
                GPIO_SetBits(GPIOA, LED_2);
                GPIO_SetBits(BUZZER_GPIO_PORT, BUZZER_GPIO_PIN);
            }
        } else {
            hold_counter_B4 = 0;
            GPIO_ResetBits(GPIOA, LED_2);
            GPIO_ResetBits(BUZZER_GPIO_PORT, BUZZER_GPIO_PIN);
        }

        Delay_ms(0.125); // delay nhỏ để tính thời gian giữ nút
    }
}

void BlinkLED(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, int times, int delay) {
    for (int i = 0; i < times; i++) {
        GPIO_SetBits(GPIOx, GPIO_Pin);
        Delay_ms(delay);
        GPIO_ResetBits(GPIOx, GPIO_Pin);
        Delay_ms(delay);
    }
}

void BuzzerBeepWithLED(int times, int delay) {
    for (int i = 0; i < times; i++) {
        GPIO_SetBits(GPIOA, LED_RED); // LED RED
        GPIO_SetBits(BUZZER_GPIO_PORT, BUZZER_GPIO_PIN); // Buzzer
        Delay_ms(delay);
        GPIO_ResetBits(GPIOA, LED_RED);
        GPIO_ResetBits(BUZZER_GPIO_PORT, BUZZER_GPIO_PIN);
        Delay_ms(delay);
    }
}

void Delay_ms(uint32_t ms) {
    for (uint32_t i = 0; i < ms * 4000; i++) {
        __NOP();
    }
}

void GPIO_Config(void) {
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

    GPIO_InitTypeDef GPIO_InitStruct;

    // LED và buzzer: PA0, PA1, PA11; PC9
    GPIO_InitStruct.GPIO_Pin = LED_GREEN | LED_RED | LED_2;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Pin = BUZZER_GPIO_PIN;
    GPIO_Init(BUZZER_GPIO_PORT, &GPIO_InitStruct);

    // Nút B2 (PB3): kéo xuống
    GPIO_InitStruct.GPIO_Pin = BUTTON_B2_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_Init(BUTTON_B2_GPIO_PORT, &GPIO_InitStruct);

    // Nút B4 (PB0): Kéo xuống
    GPIO_InitStruct.GPIO_Pin = BUTTON_B4_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_Init(BUTTON_B4_GPIO_PORT, &GPIO_InitStruct);
}
