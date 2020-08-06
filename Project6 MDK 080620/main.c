/*
Description:	Basic implementation of the Nucleo's Analog to Digital Converter functionallity
				An LED display wired to the E port will be utilized 
				to display the relative magnitude of the varying voltage input
				as produced by a photoresistor
				
Author:			Sonja Braden

Reference:		
	
IDE:			Keil uVision 5

uVision 
Dependencies:	CMSIS Core, STM32F746xx Startup files
*/

#include "stm32f746xx.h"
#include "GPIO.h"
#include "GPT.h"
#include "pinDefines.h"

// Macro defines
//#define GPIOx GPIOE


// Global variables
uint32_t core_clock_hz = 48000000;		// clockConfig() will set the Sys clock as such
uint16_t timer_ms = 100;				// timer milliseconds
volatile uint8_t LEDx = 1;				// toggled by timer interupt
uint32_t ROW_MASK = (0xFF << 2);		// LED row bit mask



// Function prototypes
void clockConfig(void);
void TIM2_IRQHandler(void);

int main(void) {
	
	
	// Configure the clock for PLL at 48 MHz
	clockConfig();
	
	// Enable the timer 2 peripheral
	enableGPT(TIM2);
	
	// Enable the TIM2 clock.
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	
	// enable clock for GPIOB peripheral
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;
	
	// For testing ***
	// RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
	
	// Set the wired LED pins to push-pull low-speed output.
	// For convenience, using pins 2 through 9, inclusive 
	for (uint8_t PINx = 2; PINx < 10; PINx++) {
		initLED(PINx, GPIOE);
	}
	
	// For testing ***
	// initLED(LED1_PIN, GPIOB);
	
	// Enable the NVIC interrupt for TIM2.
	NVIC_SetPriority(TIM2_IRQn, 0x03);
	NVIC_EnableIRQ(TIM2_IRQn);

	// Start the timer.
	initGPT(TIM2, timer_ms, core_clock_hz);
	
	
	// main event loop
	while (1) {
		
	}
}





void TIM2_IRQHandler(void) {
	// Handle a timer 'update' interrupt event
	if (TIM2->SR & TIM_SR_UIF) {
		if (LEDx == 9)
			LEDx = 1;
		else
			LEDx++;
		
		GPIOE->ODR &= ~ROW_MASK;
		GPIOE->ODR |= (1 << LEDx);
		
		// For testing ***
		//GPIOB->ODR |= (1 << LED1_PIN);

		// Reset the Update Interrupt Flag (UIF)
		TIM2->SR &= ~(TIM_SR_UIF);
	}
}


