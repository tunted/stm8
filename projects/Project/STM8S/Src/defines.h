
#ifndef _DEFINES__
#define _DEFINES__

#include "stm8s.h"

#define PWM_CYCLE       (16 * 100)

#define LED_GPIO_PORT   (GPIOB)
#define LED_GPIO_PINS   (GPIO_PIN_5 | GPIO_PIN_4)

#define PWM_PORT        GPIOC
#define PWM_CH1_PIN     GPIO_PIN_6
#define PWM_CH1N_PIN    GPIO_PIN_3
#define PWM_CH2_PIN     GPIO_PIN_7
#define PWM_CH2N_PIN    GPIO_PIN_4

// #define PWM_PINS        (PWM_CH1_PIN | PWM_CH1N_PIN |PWM_CH2_PIN | PWM_CH2N_PIN)
#define PWM_PINS        (PWM_CH2_PIN | PWM_CH2N_PIN)

#endif // _DEFINES__