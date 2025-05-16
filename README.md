# NUCLEO-F401RE-Demo
# STM32F401RE - LED & Buzzer Control with Buttons

This project demonstrates the use of GPIO to control LEDs and a buzzer on an STM32F401RE microcontroller using two buttons (B2 and B4).

## Features

- Blinks a green LED (PA5) 4 times on startup to indicate power-on.
- Pressing button B2 (PB3) 4 times triggers:
  - Red LED (PA1) blinks 4 times
  - Buzzer (PC9) beeps along with LED
- Holding button B4 (PB0) for more than ~500ms turns on:
  - Green LED 2 (PA11)
  - Buzzer (PC9)

## Hardware Configuration

| Peripheral     | Pin     | Function               |
|----------------|---------|------------------------|
| LED Green      | PA5     | Power LED              |
| LED Red        | PA1     | RGB LED - Red          |
| LED 2 Green    | PA11    | Secondary indicator    |
| Button B2      | PB3     | Press to trigger event |
| Button B4      | PB0     | Hold to trigger event  |
| Buzzer         | PC9     | Audio output           |

## Software Components

- STM32 Standard Peripheral Library
- Custom delay function using `NOP`
- GPIO configuration for input and output
- Button debounce handling
- Simple timer logic using software delays

## File Structure

```text
.
├── main.c              # Main application code
├── stm32f401re.h       # MCU register definitions
├── stm32f401re_gpio.h  # GPIO peripheral abstraction
├── stm32f401re_rcc.h   # RCC peripheral abstraction
└── README.md           # This documentation file
