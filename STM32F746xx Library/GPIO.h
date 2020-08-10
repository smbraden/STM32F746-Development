#ifndef GPIO_H
#define GPIO_H

// MCU-specific
#include "stm32f746xx.h"


typedef enum {
	GPIO_input = 1,
	GPIO_output = 0
} GPIO_mode;


// Macros
#define enablePeriphClock(AHBxENR, RCC_AHB1ENR_GPIOxEN) RCC->AHBxENR |= RCC_AHB1ENR_GPIOxEN
#define digitalRead(GPIOx, PINx) ((~(GPIOx->IDR) & (1 << PINx)) == 0 ? 0 : 1)	// returns 1 for high, 0 for low input
#define Toggle(GPIOx, PINx) (GPIOx->ODR ^= (1 << PINx))
#define digitalWrite(GPIOx, PINx, val){\
	if (val == 0)	GPIOx->ODR &= ~(1 << PINx);\
	else			GPIOx->ODR |= (1 << PINx);\
}


// Functions
void configGPIOPin(GPIO_TypeDef*, uint8_t, GPIO_mode);


#endif
