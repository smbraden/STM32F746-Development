#include "GPIO.h"

// Select port, pin number, and mode (I/O)
void configGPIOPin(GPIO_TypeDef* GPIOx, uint8_t PINx, GPIO_mode mode) {

	// enable clock for given GPIOx peripheral
	if 		(GPIOx == GPIOA)
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	else if (GPIOx == GPIOB)
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
	else if (GPIOx == GPIOC)
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
	else if (GPIOx == GPIOD)
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
	else if (GPIOx == GPIOE)
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;
	else if (GPIOx == GPIOF)
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOFEN;
	else if (GPIOx == GPIOG)
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOGEN;
	else if (GPIOx == GPIOH)
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOHEN;
	else if (GPIOx == GPIOI)
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOIEN;
	else if (GPIOx == GPIOJ)
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOJEN;
	else if (GPIOx == GPIOK)
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOKEN;

	if(mode == GPIO_output) {
			
		// Set pin to push-pull on 'output'.
		GPIOx->MODER &= ~(0x3UL << (PINx*2));		// input mode (reset state)
		GPIOx->MODER |= (0x1UL << (PINx*2));		// output mode
		GPIOx->OTYPER &= ~(1UL << PINx);			// Push-pull output
		GPIOx->PUPDR &= ~(3UL << (PINx*2));			// No pull-up/pull-down resistors
	}
	else if (mode == GPIO_input) {
		
		// Set pin to with pull-up resistors on 'input' mode
		GPIOx->MODER &= ~(0x3UL << (PINx*2));		// input mode (reset state)
		GPIOx->PUPDR &= ~(0x3UL << (PINx*2));		// no pull-up or pull-down (reset)
		GPIOx->PUPDR |= (0x1UL << (PINx*2));		// pull-up activated
	}
}



