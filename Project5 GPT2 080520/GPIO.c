#include "GPIO.h"

void enableClockGPIOB(void) {
	
	// Enable the GPIOB peripheral
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
}

void configLED(uint8_t LED_PIN, GPIO_TypeDef* GPIOx) {
	
	// Set LED pin to push-pull low-speed output.
	GPIOx->MODER  &= ~(0x3UL << (LED_PIN*2));		// input mode (reset state)
	GPIOx->MODER  |=  (0x1UL << (LED_PIN*2));		// output mode
	GPIOx->OTYPER &= ~(0x1UL << LED_PIN);			// Push-pull output
	GPIOx->PUPDR   &= ~(0x3UL << (LED_PIN*2));		// No pull-up/pull-down resistors
}

void configButton(uint8_t BUTTON_PIN, GPIO_TypeDef* GPIOx) {

	//Initialize the GPIOC pins.
	GPIOx->MODER &= ~(0x3UL << (BUTTON_PIN * 2));	// input mode (reset state)
	GPIOx->PUPDR &= ~(0x3UL << (BUTTON_PIN * 2));	// no pull-up or pull-down (reset)
	GPIOx->PUPDR |= (0x1UL << (BUTTON_PIN * 2));	// pull-up activated
	
}
