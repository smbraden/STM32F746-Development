#ifndef GPIO_H
#define GPIO_H

#include "stm32f746xx.h"

#define enablePeriphClock(AHBxENR, RCC_AHB1ENR_GPIOxEN) RCC->AHBxENR |= RCC_AHB1ENR_GPIOxEN;

// return 1 for high, 0 for low input
#define InputPinStatus(GPIOx, GPIO_Pin) ((~(GPIOx->IDR) & (1 << GPIO_Pin)) == 0 ? 0 : 1)
#define OutputHigh(GPIOx, GPIO_Pin)
#define OutputLow(GPIOx, GPIO_Pin) (GPIOx->ODR &= ~(1 << GPIO_Pin))
#define OutputToggle(GPIOx, GPIO_Pin) (GPIOx->ODR ^= (1 << GPIO_Pin))

void configLED(uint8_t, GPIO_TypeDef*);
void configButton(uint8_t, GPIO_TypeDef*);

#endif
