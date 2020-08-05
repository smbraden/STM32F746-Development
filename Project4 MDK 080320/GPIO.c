#include "stm32f746xx.h"
#include "GPIO.h"

void RCC_GPIOB() {
	
	// Enable the GPIOB peripheral
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
}

void initLED(uint8_t LED_PIN, GPIO_TypeDef* GPIOx) {
	
	// Set LED pin to push-pull low-speed output.
	GPIOx->MODER  &= ~(0x3 << (LED_PIN*2));	// input mode (reset state)
	GPIOx->MODER  |=  (0x1 << (LED_PIN*2));	// output mode
	GPIOx->OTYPER &= ~(1 << LED_PIN);			// Push-pull output
}

void initButton(uint8_t BUTTON_PIN, GPIO_TypeDef* GPIOx) {

	//Initialize the GPIOC pins.
	//PC13 should be set to 'input' mode with pull-up.
	GPIOx->MODER &= ~(0x3 << (BUTTON_PIN*2));	// input mode (reset state)
	GPIOx->PUPDR &= ~(0x3 << (BUTTON_PIN*2));	// no pull-up or pull-down (reset)
	GPIOx->PUPDR |= (0x1 << (BUTTON_PIN*2));	// pull-up activated
	
}
