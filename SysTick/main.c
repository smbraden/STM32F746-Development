/*
Description:	Basic implementation of the Sytem timer, SysTick
				
Author:			Sonja Braden
	
IDE:			Keil uVision 5

uVision 
Dependencies:	CMSIS Core, STM32F4xx/STM32F401xE Startup files
*/

#include "stm32f746xx.h"
#include "pinDefines.h"		// For configLED(LEDx, GPIOx), OutputHigh(GPIOx, PINx)

// Global variables
volatile uint32_t msTicks = 0;	// store millisecond ticks. Won't overflow for ~50 days

// Function prototypes
void configLED(uint8_t, GPIO_TypeDef*);
void initSysTick(void);
void SysTick_Handler(void);
void delay_ms(uint32_t);


int main(void) {
	
	// enable GPIOB peripheral clock
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
	
	configLED(LED1, GPIOB);		// configure the GPIO
	initSysTick();				// start system counter
	
	while (1) {
		
		GPIOB->ODR ^= (0x1UL << LED1);
		delay_ms(1000);
	}
}

//-------------GPIO Functions------------------------//

void configLED(uint8_t LED_PIN, GPIO_TypeDef* GPIOx) {
	
	// Set LED pin to push-pull low-speed output.
	GPIOx->MODER &= ~(0x3UL << (LED_PIN*2));		// input mode (reset state)
	GPIOx->MODER |= (0x1UL << (LED_PIN*2));		// output mode
	GPIOx->OTYPER &= ~(1UL << LED_PIN);			// Push-pull output
	GPIOx->PUPDR &= ~(3UL << (LED_PIN*2));		// No pull-up/pull-down resistors
}

//-----------SysTick Functipons----------------------//

// Configures SysTick to generate 1 ms interrupts
void initSysTick(void) {
	
	// 1 interrupt per millisecond
	uint32_t error = SysTick_Config(SystemCoreClock / 1000);	
	// generating 1 interupt per (SystemCoreClock / 1000) 'ticks'

}


// SysTick interrupt Handler
// Will only response to its interrupt if initSysTick() is called first 
void SysTick_Handler(void)  {
	msTicks++;	
}


// Can only be called if initSysTick() is called first
void delay_ms(uint32_t delayTime) {
	
	uint32_t curTicks;
	curTicks = msTicks;
	while ((msTicks - curTicks) < delayTime) {}
}




