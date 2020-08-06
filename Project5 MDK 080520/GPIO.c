#include "GPIO.h"

void enableClockGPIOB(void) {
	
	// Enable the GPIOB peripheral
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
}

void configLED(uint8_t LED_PIN, GPIO_TypeDef* GPIOx) {
	
	// Set LED pin to push-pull low-speed output.
	GPIOx->MODER  &= ~(0x3 << (LED_PIN*2));		// input mode (reset state)
	GPIOx->MODER  |=  (0x1 << (LED_PIN*2));		// output mode
	GPIOx->OTYPER &= ~(1 << LED_PIN);			// Push-pull output
	GPIOx->PUPDR   &= ~(3 << (LED_PIN*2));		// No pull-up/pull-down resistors
}

void configButton(uint8_t BUTTON_PIN, GPIO_TypeDef* GPIOx) {

	//Initialize the GPIOC pins.
	GPIOx->MODER &= ~(0x3 << (BUTTON_PIN * 2));	// input mode (reset state)
	GPIOx->PUPDR &= ~(0x3 << (BUTTON_PIN * 2));	// no pull-up or pull-down (reset)
	GPIOx->PUPDR |= (0x1 << (BUTTON_PIN * 2));	// pull-up activated
	
}

configAnalogOut(uint8_t PINx, GPIO_TypeDef* GPIOx) {

	RCC->AHB2ENR |= RCC_AHB2ENR_ADCEN;
	RCC->CCIPR &= ~RCC_CCIPR_ADCSEL;
	RCC->CCIPR |= (3 << RCC_CCIPR_ADCSEL_Pos);

	GPIOx->OTYPER &= ~(0x1 << 1);
	GPIOx->PUPDR &= ~(0x3 << (PINx * 2));
	GPIOx->OSPEEDR &= ~(0x3 << (PINx * 2));
	GPIOx->MODER &= ~(0x3 << (PINx * 2));
	GPIOx->MODER |= (0x3 << (PINx * 2));
}	